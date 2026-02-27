#include "SqlResource.hpp"

#include <QFile>
#include <QTextStream>

#include "Utility/Unrecoverable.hpp"

static std::string SqlResourceFileToString(const QString& QtResourcePath) {
    QFile File{ QtResourcePath };
    if (!File.open(QIODevice::ReadOnly)) {
        Unrecoverable::Throw();
    }
    return QTextStream{ &File }.readAll().toStdString();
}

std::string App::SqlResource::InitializeSchemaSql() {
    return SqlResourceFileToString(":/Memly/Migrations/Schema.sql");
}
