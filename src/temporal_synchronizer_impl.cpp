#include <algorithm>
#include <chrono>
#include <chronovyan/temporal_synchronizer_simple.hpp>
#include <random>

namespace chronovyan {
namespace sync {

TemporalSynchronizer::TemporalSynchronizer()
    : rng(std::random_device{}()), running(true), sync_requested(false) {
  // Start the synchronization thread
  sync_thread = std::thread(&TemporalSynchronizer::synchronization_loop, this);
}

TemporalSynchronizer::~TemporalSynchronizer() { stop(); }

void TemporalSynchronizer::stop() {
  {
    std::lock_guard<std::mutex> lock(sync_mutex);
    running = false;
    sync_cv.notify_all();
  }

  if (sync_thread.joinable()) {
    sync_thread.join();
  }
}

void TemporalSynchronizer::set_sync_callback(
    std::function<void(double)> callback) {
  std::lock_guard<std::mutex> lock(sync_mutex);
  sync_callback = std::move(callback);
}

void TemporalSynchronizer::synchronize_temporal_flows() {
  std::lock_guard<std::mutex> lock(sync_mutex);
  sync_requested = true;
  sync_cv.notify_one();
}

double TemporalSynchronizer::get_overall_sync() const {
  std::lock_guard<std::mutex> lock(sync_mutex);
  return sync_metrics.overall_sync;
}

double TemporalSynchronizer::get_overall_stability() const {
  std::lock_guard<std::mutex> lock(sync_mutex);
  return sync_metrics.overall_stability;
}

double TemporalSynchronizer::get_overall_coherence() const {
  std::lock_guard<std::mutex> lock(sync_mutex);
  return sync_metrics.overall_coherence;
}

void TemporalSynchronizer::synchronization_loop() {
  while (true) {
    std::unique_lock<std::mutex> lock(sync_mutex);

    // Wait for a sync request or shutdown
    sync_cv.wait(lock, [this] { return sync_requested || !running; });

    if (!running) {
      break;
    }

    // Reset the sync request
    sync_requested = false;

    // Release the lock while performing the sync operation
    lock.unlock();

    // Perform the synchronization
    perform_synchronization();
  }
}

void TemporalSynchronizer::perform_synchronization() {
  // Simulate some work
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // Update metrics with some random but reasonable values
  std::uniform_real_distribution<double> dist(0.85, 0.99);

  std::lock_guard<std::mutex> lock(sync_mutex);
  sync_metrics.overall_sync = dist(rng);
  sync_metrics.overall_stability = dist(rng);
  sync_metrics.overall_coherence = dist(rng);

  // Notify the callback if set
  if (sync_callback) {
    sync_callback(1.0); // 100% complete
  }
}

} // namespace sync
} // namespace chronovyan
