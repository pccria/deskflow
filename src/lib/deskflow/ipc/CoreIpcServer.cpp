/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2025-2026 Synergy App Ltd
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "CoreIpcServer.h"

#include "base/Log.h"
#include "common/Constants.h"

#include <QLocalSocket>

namespace deskflow::core::ipc {

static CoreIpcServer *s_instance = nullptr;

CoreIpcServer::CoreIpcServer(QObject *parent) : IpcServer(parent, kCoreIpcName, QStringLiteral("core"))
{
  assert(s_instance == nullptr);
  s_instance = this;
}

CoreIpcServer &CoreIpcServer::instance()
{
  assert(s_instance != nullptr);
  return *s_instance;
}

void CoreIpcServer::processCommand(QLocalSocket *clientSocket, const QString &command, const QStringList &parts)
{
  if (command == QStringLiteral("stop")) {
    LOG_DEBUG("core ipc server got stop message");
    writeToClientSocket(clientSocket, QStringLiteral("ok"));
    broadcastCommand(QStringLiteral("bye"));
    Q_EMIT stopProcessRequested();
    return;
  }

  if (command == QStringLiteral("switchToScreen")) {
    if (parts.size() >= 2 && !parts.at(1).isEmpty()) {
      const auto &screen = parts.at(1);
      LOG_DEBUG("core ipc server got switchToScreen: %s", screen.toUtf8().constData());
      Q_EMIT switchToScreenRequested(screen);
      writeToClientSocket(clientSocket, QStringLiteral("ok"));
      return;
    } else {
      LOG_WARN("core ipc server switchToScreen missing screen name");
      writeToClientSocket(clientSocket, QStringLiteral("error=missing screen name"));
      return;
    }
  }

  if (command == QStringLiteral("switchInDirection")) {
    if (parts.size() >= 2 && !parts.at(1).isEmpty()) {
      const auto &dir = parts.at(1);
      LOG_DEBUG("core ipc server got switchInDirection: %s", dir.toUtf8().constData());
      Q_EMIT switchInDirectionRequested(dir);
      writeToClientSocket(clientSocket, QStringLiteral("ok"));
      return;
    } else {
      LOG_WARN("core ipc server switchInDirection missing direction");
      writeToClientSocket(clientSocket, QStringLiteral("error=missing direction"));
      return;
    }
  }

  LOG_WARN("core ipc server got unknown command: %s", command.toUtf8().constData());
  writeToClientSocket(clientSocket, QStringLiteral("error=unknown command"));
}

} // namespace deskflow::core::ipc
