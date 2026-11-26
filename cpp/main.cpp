#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QSaveFile>
#include <QStringList>

#include <fstream>
#include <string>

#include "appMainWindow.h"
#include "fileManager.h"
#include "restaurantManager.h"

namespace {
QString findDataDirectory() {
    QStringList candidates;
    QDir currentDirectory(QDir::currentPath());
    candidates << currentDirectory.absoluteFilePath("data");

    QDir applicationDirectory(QCoreApplication::applicationDirPath());
    QDir searchDirectory = applicationDirectory;
    for (int level = 0; level < 8; ++level) {
        candidates << searchDirectory.absoluteFilePath("data");
        if (!searchDirectory.cdUp())
            break;
    }

    for (const QString& candidate : candidates) {
        if (QDir(candidate).exists())
            return QDir(candidate).absolutePath();
    }

    const QString fallback = QDir::currentPath() + "/data";
    QDir().mkpath(fallback);
    return QDir(fallback).absolutePath();
}

bool copyResourceIfMissing(const QString& resourcePath, const QString& targetPath) {
    if (QFileInfo::exists(targetPath))
        return true;

    QFile source(resourcePath);
    if (!source.open(QIODevice::ReadOnly))
        return false;

    QSaveFile target(targetPath);
    if (!target.open(QIODevice::WriteOnly))
        return false;

    if (target.write(source.readAll()) < 0) {
        target.cancelWriting();
        return false;
    }
    return target.commit();
}

bool ensureInitialDatabase(const QString& databasePath) {
    return copyResourceIfMissing(":/defaults/restaurant_database.txt", databasePath);
}

void ensureWritableFiles(const QString& dataDirectory) {
    const QString logPath = QDir(dataDirectory).absoluteFilePath("restaurant_actions.log");
    const QString reportPath = QDir(dataDirectory).absoluteFilePath("restaurant_report.txt");
    if (!QFileInfo::exists(logPath)) {
        QFile file(logPath);
        file.open(QIODevice::WriteOnly);
        file.close();
    }
    if (!QFileInfo::exists(reportPath)) {
        QFile file(reportPath);
        file.open(QIODevice::WriteOnly);
        file.close();
    }
}
}

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    QApplication::setApplicationName("Arcanum Restaurant");
    QApplication::setOrganizationName("Arcanum");
    QApplication::setApplicationDisplayName("ARCANUM — Beyond the Stars");

    QFile styleFile(":/styles/arcanum.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
        application.setStyleSheet(QString::fromUtf8(styleFile.readAll()));

    const QString dataDirectory = findDataDirectory();
    if (!QDir::setCurrent(QFileInfo(dataDirectory).absolutePath())) {
        QMessageBox::critical(nullptr, "Startup error", "Could not set the application working directory.");
        return 1;
    }
    const QString databasePath = QDir(dataDirectory).absoluteFilePath("restaurant_database.txt");
    const QString credentialsPath = QDir(dataDirectory).absoluteFilePath("manager_credentials.txt");

    if (!QDir().mkpath(dataDirectory)|| !ensureInitialDatabase(databasePath)|| !copyResourceIfMissing(":/defaults/manager_credentials.txt", credentialsPath)) {
        QMessageBox::critical(nullptr, "Startup error", "Could not prepare the data folder or initial files.\nCheck write permissions for the project folder.");
        return 1;
    }

    ensureWritableFiles(dataDirectory);

    RestaurantManager manager;
    FileManager fileManager("data/restaurant_database.txt", "data/restaurant_actions.log", "data/restaurant_report.txt");

    try {
        fileManager.loadDatabase(manager);
        if (manager.getDiningRoom().getTableCount() == 0) {
            manager.addTable(1, 2);
            manager.addTable(2, 2);
            manager.addTable(3, 4);
            manager.addTable(4, 4);
            manager.addTable(5, 4);
            manager.addTable(6, 6);
            manager.addTable(7, 8);
            fileManager.saveDatabase(manager);
        }
        fileManager.writeLog("Application started");
    }
    catch (const std::exception& error) {
        QMessageBox::critical(nullptr, "Database loading error", QString::fromUtf8(error.what()) + "\n\nFile: " + databasePath + "\nThe application will not overwrite a damaged database automatically.");
        return 1;
    }

    AppMainWindow window(manager, fileManager, dataDirectory);
    window.show();
    return application.exec();
}
