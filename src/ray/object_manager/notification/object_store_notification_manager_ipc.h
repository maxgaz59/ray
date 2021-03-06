// Copyright 2017 The Ray Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <list>
#include <memory>
#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <boost/bind.hpp>

#include "ray/common/client_connection.h"
#include "ray/common/id.h"
#include "ray/common/status.h"
#include "ray/object_manager/notification/object_store_notification_manager.h"
#include "ray/object_manager/plasma/client.h"

namespace ray {

/// \class ObjectStoreNotificationManagerIPC
///
/// Encapsulates notification handling from the object store.
class ObjectStoreNotificationManagerIPC : public ObjectStoreNotificationManager {
 public:
  /// Constructor.
  ///
  /// \param io_service The asio service to be used.
  /// \param store_socket_name The store socket to connect to.
  /// \param exit_on_error The manager will exit with error when it fails
  ///                      to process messages from socket.
  ObjectStoreNotificationManagerIPC(boost::asio::io_service &io_service,
                                    const std::string &store_socket_name,
                                    bool exit_on_error = true);

  ~ObjectStoreNotificationManagerIPC() override;

  /// Explicitly shutdown the manager.
  void Shutdown();

 private:
  /// Async loop for handling object store notifications.
  void NotificationWait();
  void ProcessStoreLength(const boost::system::error_code &error);
  void ProcessStoreNotification(const boost::system::error_code &error);

  plasma::PlasmaClient store_client_;
  int64_t length_;
  std::vector<uint8_t> notification_;
  local_stream_socket socket_;

  /// Flag to indicate whether or not to exit the process when received socket
  /// error. When it is false, socket error will be ignored. This flag is needed
  /// when running object store notification manager in core worker. On core worker
  /// exit, plasma store will be killed before deconstructor of this manager. So we
  /// we have to silence the errors.
  bool exit_on_error_;
};

}  // namespace ray
