#include <algorithm>
#include <numeric>

#include "../include/temporal_debt_tracker.h"

namespace chronovyan {

// OldestFirstStrategy implementation
RepaymentResult OldestFirstStrategy::apply(std::vector<DebtRecord> &debts,
                                           double available_chronons,
                                           double available_aethel) const {
  RepaymentResult result;

  // Sort debts by timestamp (oldest first)
  std::sort(debts.begin(), debts.end(),
            [](const DebtRecord &a, const DebtRecord &b) {
              return a.timestamp < b.timestamp;
            });

  // Track remaining resources
  double remaining_chronons = available_chronons;
  double remaining_aethel = available_aethel;

  // Iterate through debts and repay as much as possible
  for (auto &debt : debts) {
    if (remaining_chronons <= 0 && remaining_aethel <= 0) {
      break; // No more resources available
    }

    // Determine amount to repay
    double chronon_to_repay = std::min(remaining_chronons, debt.chronon_debt);
    double aethel_to_repay = std::min(remaining_aethel, debt.aethel_debt);

    // Calculate interest saved
    double interest_saved =
        calculateInterestSaved(debt, chronon_to_repay, aethel_to_repay);

    // Update debt
    debt.chronon_debt -= chronon_to_repay;
    debt.aethel_debt -= aethel_to_repay;

    // Update result
    result.chronons_repaid += chronon_to_repay;
    result.aethel_repaid += aethel_to_repay;
    result.stability_improvement +=
        interest_saved * 0.05; // Scale factor for stability

    // Update remaining resources
    remaining_chronons -= chronon_to_repay;
    remaining_aethel -= aethel_to_repay;

    // Check if debt is fully repaid
    if (debt.chronon_debt <= 0 && debt.aethel_debt <= 0) {
      result.debts_cleared++;
    }
  }

  // Remove fully repaid debts
  debts.erase(std::remove_if(debts.begin(), debts.end(),
                             [](const DebtRecord &debt) {
                               return debt.chronon_debt <= 0 &&
                                      debt.aethel_debt <= 0;
                             }),
              debts.end());

  return result;
}

// HighestInterestStrategy implementation
RepaymentResult HighestInterestStrategy::apply(std::vector<DebtRecord> &debts,
                                               double available_chronons,
                                               double available_aethel) const {
  RepaymentResult result;

  // Sort debts by interest rate (highest first)
  std::sort(debts.begin(), debts.end(),
            [](const DebtRecord &a, const DebtRecord &b) {
              return a.interest_rate > b.interest_rate;
            });

  // Track remaining resources
  double remaining_chronons = available_chronons;
  double remaining_aethel = available_aethel;

  // Iterate through debts and repay as much as possible
  for (auto &debt : debts) {
    if (remaining_chronons <= 0 && remaining_aethel <= 0) {
      break; // No more resources available
    }

    // Determine amount to repay
    double chronon_to_repay = std::min(remaining_chronons, debt.chronon_debt);
    double aethel_to_repay = std::min(remaining_aethel, debt.aethel_debt);

    // Calculate interest saved
    double interest_saved =
        calculateInterestSaved(debt, chronon_to_repay, aethel_to_repay);

    // Update debt
    debt.chronon_debt -= chronon_to_repay;
    debt.aethel_debt -= aethel_to_repay;

    // Update result
    result.chronons_repaid += chronon_to_repay;
    result.aethel_repaid += aethel_to_repay;
    result.stability_improvement +=
        interest_saved * 0.05; // Scale factor for stability

    // Update remaining resources
    remaining_chronons -= chronon_to_repay;
    remaining_aethel -= aethel_to_repay;

    // Check if debt is fully repaid
    if (debt.chronon_debt <= 0 && debt.aethel_debt <= 0) {
      result.debts_cleared++;
    }
  }

  // Remove fully repaid debts
  debts.erase(std::remove_if(debts.begin(), debts.end(),
                             [](const DebtRecord &debt) {
                               return debt.chronon_debt <= 0 &&
                                      debt.aethel_debt <= 0;
                             }),
              debts.end());

  return result;
}

// CriticalFirstStrategy implementation
RepaymentResult CriticalFirstStrategy::apply(std::vector<DebtRecord> &debts,
                                             double available_chronons,
                                             double available_aethel) const {
  RepaymentResult result;

  // Sort debts by critical flag (critical first) and then by due cycle
  // (earliest first)
  std::sort(debts.begin(), debts.end(),
            [](const DebtRecord &a, const DebtRecord &b) {
              if (a.critical != b.critical) {
                return a.critical > b.critical; // True before false
              }
              return a.due_cycle < b.due_cycle; // Earlier due cycle first
            });

  // Track remaining resources
  double remaining_chronons = available_chronons;
  double remaining_aethel = available_aethel;

  // Iterate through debts and repay as much as possible
  for (auto &debt : debts) {
    if (remaining_chronons <= 0 && remaining_aethel <= 0) {
      break; // No more resources available
    }

    // Determine amount to repay
    double chronon_to_repay = std::min(remaining_chronons, debt.chronon_debt);
    double aethel_to_repay = std::min(remaining_aethel, debt.aethel_debt);

    // Calculate interest saved
    double interest_saved =
        calculateInterestSaved(debt, chronon_to_repay, aethel_to_repay);

    // Update debt
    debt.chronon_debt -= chronon_to_repay;
    debt.aethel_debt -= aethel_to_repay;

    // Update result
    result.chronons_repaid += chronon_to_repay;
    result.aethel_repaid += aethel_to_repay;
    result.stability_improvement +=
        interest_saved *
        (debt.critical ? 0.1 : 0.05); // Higher factor for critical debts

    // Update remaining resources
    remaining_chronons -= chronon_to_repay;
    remaining_aethel -= aethel_to_repay;

    // Check if debt is fully repaid
    if (debt.chronon_debt <= 0 && debt.aethel_debt <= 0) {
      result.debts_cleared++;
    }
  }

  // Remove fully repaid debts
  debts.erase(std::remove_if(debts.begin(), debts.end(),
                             [](const DebtRecord &debt) {
                               return debt.chronon_debt <= 0 &&
                                      debt.aethel_debt <= 0;
                             }),
              debts.end());

  return result;
}

// BalancedStrategy implementation
RepaymentResult BalancedStrategy::apply(std::vector<DebtRecord> &debts,
                                        double available_chronons,
                                        double available_aethel) const {
  RepaymentResult result;

  if (debts.empty()) {
    return result; // No debts to repay
  }

  // Calculate total debt for both resources
  double total_chronon_debt = std::accumulate(
      debts.begin(), debts.end(), 0.0, [](double sum, const DebtRecord &debt) {
        return sum + debt.chronon_debt;
      });

  double total_aethel_debt = std::accumulate(
      debts.begin(), debts.end(), 0.0, [](double sum, const DebtRecord &debt) {
        return sum + debt.aethel_debt;
      });

  // Distribute repayment proportionally to debt sizes
  for (auto &debt : debts) {
    // Calculate proportion of total debt
    double chronon_proportion = (total_chronon_debt > 0)
                                    ? (debt.chronon_debt / total_chronon_debt)
                                    : 0.0;
    double aethel_proportion =
        (total_aethel_debt > 0) ? (debt.aethel_debt / total_aethel_debt) : 0.0;

    // Calculate repayment amounts
    double chronon_to_repay =
        std::min(debt.chronon_debt, available_chronons * chronon_proportion);
    double aethel_to_repay =
        std::min(debt.aethel_debt, available_aethel * aethel_proportion);

    // Calculate interest saved
    double interest_saved =
        calculateInterestSaved(debt, chronon_to_repay, aethel_to_repay);

    // Update debt
    debt.chronon_debt -= chronon_to_repay;
    debt.aethel_debt -= aethel_to_repay;

    // Update result
    result.chronons_repaid += chronon_to_repay;
    result.aethel_repaid += aethel_to_repay;
    result.stability_improvement +=
        interest_saved * 0.05; // Scale factor for stability

    // Check if debt is fully repaid
    if (debt.chronon_debt <= 0 && debt.aethel_debt <= 0) {
      result.debts_cleared++;
    }
  }

  // Remove fully repaid debts
  debts.erase(std::remove_if(debts.begin(), debts.end(),
                             [](const DebtRecord &debt) {
                               return debt.chronon_debt <= 0 &&
                                      debt.aethel_debt <= 0;
                             }),
              debts.end());

  return result;
}

// MinimumPaymentsStrategy implementation
RepaymentResult MinimumPaymentsStrategy::apply(std::vector<DebtRecord> &debts,
                                               double available_chronons,
                                               double available_aethel) const {
  RepaymentResult result;

  // Calculate minimum payment per debt
  double min_chronon_payment =
      available_chronons / std::max(1.0, static_cast<double>(debts.size()));
  double min_aethel_payment =
      available_aethel / std::max(1.0, static_cast<double>(debts.size()));

  // Track remaining resources
  double remaining_chronons = available_chronons;
  double remaining_aethel = available_aethel;

  // Make minimum payments on all debts
  for (auto &debt : debts) {
    if (remaining_chronons <= 0 && remaining_aethel <= 0) {
      break; // No more resources available
    }

    // Determine amount to repay (minimum or full debt if less)
    double chronon_to_repay = std::min(
        remaining_chronons, std::min(min_chronon_payment, debt.chronon_debt));
    double aethel_to_repay = std::min(
        remaining_aethel, std::min(min_aethel_payment, debt.aethel_debt));

    // Calculate interest saved
    double interest_saved =
        calculateInterestSaved(debt, chronon_to_repay, aethel_to_repay);

    // Update debt
    debt.chronon_debt -= chronon_to_repay;
    debt.aethel_debt -= aethel_to_repay;

    // Update result
    result.chronons_repaid += chronon_to_repay;
    result.aethel_repaid += aethel_to_repay;
    result.stability_improvement +=
        interest_saved * 0.05; // Scale factor for stability

    // Update remaining resources
    remaining_chronons -= chronon_to_repay;
    remaining_aethel -= aethel_to_repay;

    // Check if debt is fully repaid
    if (debt.chronon_debt <= 0 && debt.aethel_debt <= 0) {
      result.debts_cleared++;
    }
  }

  // Remove fully repaid debts
  debts.erase(std::remove_if(debts.begin(), debts.end(),
                             [](const DebtRecord &debt) {
                               return debt.chronon_debt <= 0 &&
                                      debt.aethel_debt <= 0;
                             }),
              debts.end());

  return result;
}

// SnowballStrategy implementation
RepaymentResult SnowballStrategy::apply(std::vector<DebtRecord> &debts,
                                        double available_chronons,
                                        double available_aethel) const {
  RepaymentResult result;

  // Sort debts by total size (smallest first)
  std::sort(debts.begin(), debts.end(),
            [](const DebtRecord &a, const DebtRecord &b) {
              return (a.chronon_debt + a.aethel_debt) <
                     (b.chronon_debt + b.aethel_debt);
            });

  // Track remaining resources
  double remaining_chronons = available_chronons;
  double remaining_aethel = available_aethel;

  // Iterate through debts and repay as much as possible
  for (auto &debt : debts) {
    if (remaining_chronons <= 0 && remaining_aethel <= 0) {
      break; // No more resources available
    }

    // Determine amount to repay
    double chronon_to_repay = std::min(remaining_chronons, debt.chronon_debt);
    double aethel_to_repay = std::min(remaining_aethel, debt.aethel_debt);

    // Calculate interest saved
    double interest_saved =
        calculateInterestSaved(debt, chronon_to_repay, aethel_to_repay);

    // Update debt
    debt.chronon_debt -= chronon_to_repay;
    debt.aethel_debt -= aethel_to_repay;

    // Update result
    result.chronons_repaid += chronon_to_repay;
    result.aethel_repaid += aethel_to_repay;
    result.stability_improvement +=
        interest_saved * 0.05; // Scale factor for stability

    // Update remaining resources
    remaining_chronons -= chronon_to_repay;
    remaining_aethel -= aethel_to_repay;

    // Check if debt is fully repaid
    if (debt.chronon_debt <= 0 && debt.aethel_debt <= 0) {
      result.debts_cleared++;
    }
  }

  // Remove fully repaid debts
  debts.erase(std::remove_if(debts.begin(), debts.end(),
                             [](const DebtRecord &debt) {
                               return debt.chronon_debt <= 0 &&
                                      debt.aethel_debt <= 0;
                             }),
              debts.end());

  return result;
}

// AvalancheStrategy implementation
RepaymentResult AvalancheStrategy::apply(std::vector<DebtRecord> &debts,
                                         double available_chronons,
                                         double available_aethel) const {
  // This is similar to HighestInterestStrategy but focuses on completely
  // repaying one debt before moving to the next
  RepaymentResult result;

  // Sort debts by interest rate (highest first)
  std::sort(debts.begin(), debts.end(),
            [](const DebtRecord &a, const DebtRecord &b) {
              return a.interest_rate > b.interest_rate;
            });

  // Track remaining resources
  double remaining_chronons = available_chronons;
  double remaining_aethel = available_aethel;

  // Iterate through debts and repay fully before moving to next
  for (auto &debt : debts) {
    if (remaining_chronons <= 0 && remaining_aethel <= 0) {
      break; // No more resources available
    }

    // Determine amount to repay
    double chronon_to_repay = std::min(remaining_chronons, debt.chronon_debt);
    double aethel_to_repay = std::min(remaining_aethel, debt.aethel_debt);

    // Calculate interest saved
    double interest_saved =
        calculateInterestSaved(debt, chronon_to_repay, aethel_to_repay);

    // Update debt
    debt.chronon_debt -= chronon_to_repay;
    debt.aethel_debt -= aethel_to_repay;

    // Update result
    result.chronons_repaid += chronon_to_repay;
    result.aethel_repaid += aethel_to_repay;
    result.stability_improvement +=
        interest_saved * 0.05; // Scale factor for stability

    // Update remaining resources
    remaining_chronons -= chronon_to_repay;
    remaining_aethel -= aethel_to_repay;

    // Check if debt is fully repaid
    if (debt.chronon_debt <= 0 && debt.aethel_debt <= 0) {
      result.debts_cleared++;
    }
  }

  // Remove fully repaid debts
  debts.erase(std::remove_if(debts.begin(), debts.end(),
                             [](const DebtRecord &debt) {
                               return debt.chronon_debt <= 0 &&
                                      debt.aethel_debt <= 0;
                             }),
              debts.end());

  return result;
}

// StabilityOptimizedStrategy implementation
RepaymentResult
StabilityOptimizedStrategy::apply(std::vector<DebtRecord> &debts,
                                  double available_chronons,
                                  double available_aethel) const {
  RepaymentResult result;

  // Sort debts by a composite score that combines criticality, due date, and
  // interest rate
  std::sort(
      debts.begin(), debts.end(), [](const DebtRecord &a, const DebtRecord &b) {
        // Create a stability score where higher means more important to repay
        double a_score = (a.critical ? 100.0 : 0.0) +
                         (100.0 / (a.due_cycle + 1)) + (a.interest_rate * 20.0);
        double b_score = (b.critical ? 100.0 : 0.0) +
                         (100.0 / (b.due_cycle + 1)) + (b.interest_rate * 20.0);
        return a_score > b_score;
      });

  // Track remaining resources
  double remaining_chronons = available_chronons;
  double remaining_aethel = available_aethel;

  // Iterate through debts and repay as much as possible
  for (auto &debt : debts) {
    if (remaining_chronons <= 0 && remaining_aethel <= 0) {
      break; // No more resources available
    }

    // Determine amount to repay
    double chronon_to_repay = std::min(remaining_chronons, debt.chronon_debt);
    double aethel_to_repay = std::min(remaining_aethel, debt.aethel_debt);

    // Calculate interest saved
    double interest_saved =
        calculateInterestSaved(debt, chronon_to_repay, aethel_to_repay);

    // Update debt
    debt.chronon_debt -= chronon_to_repay;
    debt.aethel_debt -= aethel_to_repay;

    // Update result
    result.chronons_repaid += chronon_to_repay;
    result.aethel_repaid += aethel_to_repay;

    // Higher stability improvement for critical debts and those close to due
    // date
    double stability_factor = 0.05;
    if (debt.critical) {
      stability_factor *= 2.0;
    }
    if (debt.due_cycle <= 2) {
      stability_factor *= 1.5;
    }

    result.stability_improvement += interest_saved * stability_factor;

    // Update remaining resources
    remaining_chronons -= chronon_to_repay;
    remaining_aethel -= aethel_to_repay;

    // Check if debt is fully repaid
    if (debt.chronon_debt <= 0 && debt.aethel_debt <= 0) {
      result.debts_cleared++;
    }
  }

  // Remove fully repaid debts
  debts.erase(std::remove_if(debts.begin(), debts.end(),
                             [](const DebtRecord &debt) {
                               return debt.chronon_debt <= 0 &&
                                      debt.aethel_debt <= 0;
                             }),
              debts.end());

  return result;
}

// ChronosPriorityStrategy implementation
RepaymentResult ChronosPriorityStrategy::apply(std::vector<DebtRecord> &debts,
                                               double available_chronons,
                                               double available_aethel) const {
  RepaymentResult result;

  // Sort debts by chronon debt (highest first)
  std::sort(debts.begin(), debts.end(),
            [](const DebtRecord &a, const DebtRecord &b) {
              return a.chronon_debt > b.chronon_debt;
            });

  // Track remaining resources
  double remaining_chronons = available_chronons;
  double remaining_aethel = available_aethel;

  // Repay chronon debt first for all debts
  for (auto &debt : debts) {
    if (remaining_chronons <= 0) {
      break; // No more chronons available
    }

    // Determine chronon amount to repay
    double chronon_to_repay = std::min(remaining_chronons, debt.chronon_debt);

    // Calculate interest saved (for chronons only)
    double interest_saved = calculateInterestSaved(debt, chronon_to_repay, 0.0);

    // Update debt
    debt.chronon_debt -= chronon_to_repay;

    // Update result
    result.chronons_repaid += chronon_to_repay;
    result.stability_improvement +=
        interest_saved * 0.05; // Scale factor for stability

    // Update remaining resources
    remaining_chronons -= chronon_to_repay;
  }

  // Then repay aethel debt with any remaining resources
  for (auto &debt : debts) {
    if (remaining_aethel <= 0) {
      break; // No more aethel available
    }

    // Determine aethel amount to repay
    double aethel_to_repay = std::min(remaining_aethel, debt.aethel_debt);

    // Calculate interest saved (for aethel only)
    double interest_saved = calculateInterestSaved(debt, 0.0, aethel_to_repay);

    // Update debt
    debt.aethel_debt -= aethel_to_repay;

    // Update result
    result.aethel_repaid += aethel_to_repay;
    result.stability_improvement +=
        interest_saved * 0.05; // Scale factor for stability

    // Update remaining resources
    remaining_aethel -= aethel_to_repay;
  }

  // Count fully repaid debts
  for (const auto &debt : debts) {
    if (debt.chronon_debt <= 0 && debt.aethel_debt <= 0) {
      result.debts_cleared++;
    }
  }

  // Remove fully repaid debts
  debts.erase(std::remove_if(debts.begin(), debts.end(),
                             [](const DebtRecord &debt) {
                               return debt.chronon_debt <= 0 &&
                                      debt.aethel_debt <= 0;
                             }),
              debts.end());

  return result;
}

// AethelPriorityStrategy implementation
RepaymentResult AethelPriorityStrategy::apply(std::vector<DebtRecord> &debts,
                                              double available_chronons,
                                              double available_aethel) const {
  RepaymentResult result;

  // Sort debts by aethel debt (highest first)
  std::sort(debts.begin(), debts.end(),
            [](const DebtRecord &a, const DebtRecord &b) {
              return a.aethel_debt > b.aethel_debt;
            });

  // Track remaining resources
  double remaining_chronons = available_chronons;
  double remaining_aethel = available_aethel;

  // Repay aethel debt first for all debts
  for (auto &debt : debts) {
    if (remaining_aethel <= 0) {
      break; // No more aethel available
    }

    // Determine aethel amount to repay
    double aethel_to_repay = std::min(remaining_aethel, debt.aethel_debt);

    // Calculate interest saved (for aethel only)
    double interest_saved = calculateInterestSaved(debt, 0.0, aethel_to_repay);

    // Update debt
    debt.aethel_debt -= aethel_to_repay;

    // Update result
    result.aethel_repaid += aethel_to_repay;
    result.stability_improvement +=
        interest_saved * 0.05; // Scale factor for stability

    // Update remaining resources
    remaining_aethel -= aethel_to_repay;
  }

  // Then repay chronon debt with any remaining resources
  for (auto &debt : debts) {
    if (remaining_chronons <= 0) {
      break; // No more chronons available
    }

    // Determine chronon amount to repay
    double chronon_to_repay = std::min(remaining_chronons, debt.chronon_debt);

    // Calculate interest saved (for chronons only)
    double interest_saved = calculateInterestSaved(debt, chronon_to_repay, 0.0);

    // Update debt
    debt.chronon_debt -= chronon_to_repay;

    // Update result
    result.chronons_repaid += chronon_to_repay;
    result.stability_improvement +=
        interest_saved * 0.05; // Scale factor for stability

    // Update remaining resources
    remaining_chronons -= chronon_to_repay;
  }

  // Count fully repaid debts
  for (const auto &debt : debts) {
    if (debt.chronon_debt <= 0 && debt.aethel_debt <= 0) {
      result.debts_cleared++;
    }
  }

  // Remove fully repaid debts
  debts.erase(std::remove_if(debts.begin(), debts.end(),
                             [](const DebtRecord &debt) {
                               return debt.chronon_debt <= 0 &&
                                      debt.aethel_debt <= 0;
                             }),
              debts.end());

  return result;
}

} // namespace chronovyan