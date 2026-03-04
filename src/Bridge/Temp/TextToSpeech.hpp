// #include <QFile>
// #include <QJsonArray>
// #include <QJsonDocument>
// #include <QNetworkAccessManager>
// #include <QNetworkReply>
// #include <QNetworkRequest>
// #include <QQmlApplicationEngine>
// #include <QScopeGuard>
// #include <QUrlQuery>

// QNetworkAccessManager nam;

// QUrl      url("https://translate.googleapis.com/translate_a/single");
// QUrlQuery q;
// q.addQueryItem("client", "gtx");
// q.addQueryItem("sl", "auto"); // source language
// q.addQueryItem("tl", "de");   // target language
// q.addQueryItem("dt", "t");
// q.addQueryItem("q", "Hello, my name is Shawn.");
// url.setQuery(q);

// QNetworkRequest req(url);

// QNetworkReply* reply = nam.get(req);

// QObject::connect(reply, &QNetworkReply::finished, [&] {
//     const auto guard = qScopeGuard([&] { reply->deleteLater(); });

//     if (reply->error() != QNetworkReply::NoError) {
//         qWarning() << "Network error:" << reply->errorString();
//         app.quit();
//         return;
//     }

//     const QByteArray body = reply->readAll();
//     QJsonParseError  jerr{};
//     QJsonDocument    doc = QJsonDocument::fromJson(body, &jerr);
//     if (jerr.error != QJsonParseError::NoError || !doc.isArray()) {
//         qWarning() << "Invalid JSON";
//         app.quit();
//         return;
//     }

//     // Parse translation: [[["Hello World","Hallo Welt",...],...],...]
//     QJsonArray top = doc.array();
//     QString    out;
//     for (const auto& seg : top[0].toArray()) {
//         if (seg.isArray() && seg.toArray()[0].isString()) out += seg.toArray()[0].toString();
//     }
//     qDebug() << "Translation:" << out;
//     // app.quit();
// });

// QUrl      url1("https://translate.google.com/translate_tts");
// QUrlQuery q1;
// q1.addQueryItem("ie", "UTF-8");
// q1.addQueryItem("client", "tw-ob");
// q1.addQueryItem("tl", "en");         // target language
// q1.addQueryItem("q", "Hello world"); // text to speak
// url1.setQuery(q1);

// QNetworkRequest req1(url1);
// QNetworkReply*  reply1 = nam.get(req1);

// QObject::connect(reply1, &QNetworkReply::finished, [reply1]() {
//     if (reply1->error() != QNetworkReply::NoError) {
//         qWarning() << "Network error:" << reply1->errorString();
//     } else {
//         QFile outFile("speech.mp3");
//         if (!outFile.open(QIODevice::WriteOnly)) {
//             qWarning() << "Failed to open file speech.mp3 for writing";
//         } else {
//             outFile.write(reply1->readAll());
//             outFile.close();
//             qDebug() << "Saved to speech.mp3";
//         }
//     }
//     reply1->deleteLater();
// });
