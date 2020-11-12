#include "cluster_manager.h"

#include <chrono>
#include <functional>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace Envoy {
namespace Upstream {

using ::testing::_;
using ::testing::Eq;
using ::testing::Return;
using ::testing::ReturnRef;

MockClusterManager::MockClusterManager(TimeSource&) : MockClusterManager() {}

MockClusterManager::MockClusterManager() {
  ON_CALL(*this, httpConnPoolForCluster(_, _, _, _)).WillByDefault(Return(&conn_pool_));
  ON_CALL(*this, tcpConnPoolForCluster(_, _, _)).WillByDefault(Return(&tcp_conn_pool_));
  ON_CALL(*this, httpAsyncClientForCluster(_)).WillByDefault(ReturnRef(async_client_));
  ON_CALL(*this, httpAsyncClientForCluster(_)).WillByDefault((ReturnRef(async_client_)));
  ON_CALL(*this, bindConfig()).WillByDefault(ReturnRef(bind_config_));
  ON_CALL(*this, adsMux()).WillByDefault(Return(ads_mux_));
  ON_CALL(*this, grpcAsyncClientManager()).WillByDefault(ReturnRef(async_client_manager_));
  ON_CALL(*this, localClusterName()).WillByDefault((ReturnRef(local_cluster_name_)));

  // Matches are LIFO so "" will match first.
  ON_CALL(*this, get(_)).WillByDefault(Return(&thread_local_cluster_));
  ON_CALL(*this, get(Eq(""))).WillByDefault(Return(nullptr));
  ON_CALL(*this, subscriptionFactory()).WillByDefault(ReturnRef(subscription_factory_));
}

void MockClusterManager::initializeClusters(const std::vector<std::string>& active_cluster_names,
                                            const std::vector<std::string>&) {
  ClusterManager::ClusterInfoMaps info_map;
  for (const auto& name : active_cluster_names) {
    auto new_cluster = std::make_unique<NiceMock<MockCluster>>();
    new_cluster->info_->name_ = name;
    info_map.active_clusters_.emplace(name, *new_cluster);
    active_clusters_.emplace(name, std::move(new_cluster));
  }

  ON_CALL(*this, clusters()).WillByDefault(Return(info_map));
}

MockClusterManager::~MockClusterManager() = default;

} // namespace Upstream
} // namespace Envoy
