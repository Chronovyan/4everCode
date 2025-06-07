#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "temporal_debt_tracker.h"
#include "timestream.h"

namespace chronovyan {

/**
 * @class TimestreamManager
 * @brief Manages the collection of timestreams and their interactions
 *
 * The TimestreamManager serves as the central control point for the version control
 * system, managing the creation, deletion, and harmonization of timestreams. It also
 * integrates with the TemporalDebtTracker to handle debt accrual for operations.
 */
class TimestreamManager {
public:
    /**
     * @brief Constructor
     * @param debt_tracker Reference to the debt tracker for managing temporal debt
     */
    TimestreamManager(TemporalDebtTracker& debt_tracker);

    /**
     * @brief Get the main timestream
     * @return Pointer to the main timestream
     */
    std::shared_ptr<Timestream> getMainTimestream() const;

    /**
     * @brief Get the currently active timestream
     * @return Pointer to the active timestream
     */
    std::shared_ptr<Timestream> getActiveTimestream() const;

    /**
     * @brief Set the active timestream
     * @param timestream_id ID of the timestream to set as active
     * @return True if successful, false if timestream not found
     */
    bool setActiveTimestream(const std::string& timestream_id);

    /**
     * @brief Get all timestreams
     * @return Vector of pointers to all timestreams
     */
    std::vector<std::shared_ptr<Timestream>> getAllTimestreams() const;

    /**
     * @brief Find a timestream by ID
     * @param id The ID of the timestream to find
     * @return Pointer to the found timestream, or nullptr if not found
     */
    std::shared_ptr<Timestream> findTimestream(const std::string& id) const;

    /**
     * @brief Create a new timestream (Echo) from an anchor point
     * @param name Name for the new timestream
     * @param source_timestream_id ID of the source timestream
     * @param anchor_id ID of the anchor to branch from
     * @return Pointer to the newly created timestream
     * @throws std::runtime_error If source timestream or anchor not found
     */
    std::shared_ptr<Timestream> createEcho(const std::string& name,
                                           const std::string& source_timestream_id,
                                           const std::string& anchor_id);

    /**
     * @brief Create a new timestream (Echo) with a specific type
     * @param name Name for the new timestream
     * @param source_timestream_id ID of the source timestream
     * @param stream_type Type of the timestream to create
     * @return Pointer to the newly created timestream
     */
    std::shared_ptr<Timestream> createEcho(const std::string& name,
                                           const std::string& source_timestream_id,
                                           int stream_type);

    /**
     * @brief Create a new anchor in the active timestream
     * @param stability Stability value for the new anchor
     * @param description Description of the anchor
     * @return Pointer to the newly created anchor
     */
    std::shared_ptr<TimeAnchor> createAnchor(double stability, const std::string& description);

    /**
     * @brief Harmonize (merge) a timestream into another
     * @param source_id ID of the source timestream
     * @param target_id ID of the target timestream
     * @param strategy The debt repayment strategy to use for resolving conflicts
     * @return True if harmonization was successful
     * @throws std::runtime_error If source or target timestream not found
     */
    bool harmonizeTimestreams(const std::string& source_id, const std::string& target_id,
                              RepaymentStrategyType strategy = RepaymentStrategyType::BALANCED);

    /**
     * @brief Calculate the stability impact of harmonizing two timestreams
     * @param source_id ID of the source timestream
     * @param target_id ID of the target timestream
     * @return The projected stability impact (negative values indicate destabilization)
     */
    double calculateHarmonizationImpact(const std::string& source_id,
                                        const std::string& target_id) const;

    /**
     * @brief Calculate the paradox risk of creating an Echo from an anchor
     * @param timestream_id ID of the source timestream
     * @param anchor_id ID of the anchor to branch from
     * @return Paradox risk value (0.0-1.0)
     */
    double calculateEchoParadoxRisk(const std::string& timestream_id,
                                    const std::string& anchor_id) const;

    /**
     * @brief Delete a timestream
     * @param timestream_id ID of the timestream to delete
     * @return True if successful, false if timestream not found or cannot be deleted
     */
    bool deleteTimestream(const std::string& timestream_id);

private:
    TemporalDebtTracker& m_debt_tracker;  ///< Reference to debt tracker
    std::map<std::string, std::shared_ptr<Timestream>> m_timestreams;  ///< Map of all timestreams
    std::shared_ptr<Timestream> m_active_timestream;  ///< Currently active timestream
    std::shared_ptr<Timestream> m_main_timestream;    ///< The main timestream

    /**
     * @brief Initialize the main timestream
     */
    void initializeMainTimestream();

    /**
     * @brief Generate a unique ID for a new timestream
     * @return A unique timestream ID
     */
    std::string generateTimestreamId() const;

    /**
     * @brief Record debt for creating an Echo
     * @param source_timestream Source timestream
     * @param anchor Source anchor
     * @return The amount of debt accrued
     */
    double recordEchoDebt(const std::shared_ptr<Timestream> source_timestream,
                          const std::shared_ptr<TimeAnchor> anchor);

    /**
     * @brief Record debt for harmonizing timestreams
     * @param source_timestream Source timestream
     * @param target_timestream Target timestream
     * @return The amount of debt accrued
     */
    double recordHarmonizationDebt(const std::shared_ptr<Timestream> source_timestream,
                                   const std::shared_ptr<Timestream> target_timestream);
};

}  // namespace chronovyan