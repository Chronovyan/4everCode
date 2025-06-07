#pragma once

#include <memory>
#include <string>
#include <vector>

#include "time_anchor.h"

namespace chronovyan {

/**
 * @class Timestream
 * @brief Represents a specific branch of execution in the Chronovyan version control system
 *
 * A Timestream is conceptually similar to a branch in version control systems, containing
 * a series of TimeAnchors and maintaining its own stability metrics. Timestreams can
 * diverge (creating Echoes) and be harmonized (merged).
 */
class Timestream {
public:
    /**
     * @brief Constructor
     * @param id Unique identifier for this timestream
     * @param name Human-readable name for this timestream
     * @param parent Parent timestream (nullptr for main timestream)
     * @param source_anchor Anchor point where this timestream was created (nullptr for main)
     */
    Timestream(const std::string& id, const std::string& name,
               std::shared_ptr<Timestream> parent = nullptr,
               std::shared_ptr<TimeAnchor> source_anchor = nullptr);

    /**
     * @brief Get the unique identifier for this timestream
     * @return The timestream ID
     */
    std::string getId() const;

    /**
     * @brief Get the name of this timestream
     * @return The timestream name
     */
    std::string getName() const;

    /**
     * @brief Set the name of this timestream
     * @param name The new name
     */
    void setName(const std::string& name);

    /**
     * @brief Get the parent timestream
     * @return Pointer to the parent timestream, or nullptr if this is the main timestream
     */
    std::shared_ptr<Timestream> getParent() const;

    /**
     * @brief Get the source anchor for this timestream
     * @return Pointer to the source anchor, or nullptr if this is the main timestream
     */
    std::shared_ptr<TimeAnchor> getSourceAnchor() const;

    /**
     * @brief Create a new anchor in this timestream
     * @param stability Stability value for the new anchor
     * @param description Description of the anchor
     * @return Pointer to the newly created anchor
     */
    std::shared_ptr<TimeAnchor> createAnchor(double stability, const std::string& description);

    /**
     * @brief Get all anchors in this timestream
     * @return Vector of pointers to anchors
     */
    std::vector<std::shared_ptr<TimeAnchor>> getAnchors() const;

    /**
     * @brief Get the most recent anchor in this timestream
     * @return Pointer to the most recent anchor, or nullptr if no anchors exist
     */
    std::shared_ptr<TimeAnchor> getLatestAnchor() const;

    /**
     * @brief Find an anchor by ID
     * @param id The ID of the anchor to find
     * @return Pointer to the found anchor, or nullptr if not found
     */
    std::shared_ptr<TimeAnchor> findAnchor(const std::string& id) const;

    /**
     * @brief Calculate the overall stability of this timestream
     * @return Stability value (0.0-1.0)
     */
    double calculateStability() const;

    /**
     * @brief Check if this timestream is derived from another
     * @param other The timestream to check against
     * @return True if this timestream is derived from the other
     */
    bool isDerivedFrom(const std::shared_ptr<Timestream> other) const;

    /**
     * @brief Calculate the divergence between this timestream and another
     * @param other The timestream to compare with
     * @return Divergence value (higher means more different)
     */
    double calculateDivergence(const std::shared_ptr<Timestream> other) const;

private:
    std::string m_id;                             ///< Unique identifier
    std::string m_name;                           ///< Human-readable name
    std::shared_ptr<Timestream> m_parent;         ///< Parent timestream
    std::shared_ptr<TimeAnchor> m_source_anchor;  ///< Anchor where this timestream was created
    std::vector<std::shared_ptr<TimeAnchor>> m_anchors;  ///< Anchors in this timestream

    /**
     * @brief Generate a unique ID for a new anchor
     * @return A unique anchor ID
     */
    std::string generateAnchorId() const;
};

}  // namespace chronovyan