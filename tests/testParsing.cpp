#include <QJsonObject>

#include "testTool.hpp"
#include "parsingTool.hpp"
#include "payloadTool.hpp"

namespace
{
    const QString websiteOfJvc = "www.jeuxvideo.com";

    infoForMessageParsingStruct makeBaseInfoForParsing()
    {
        infoForMessageParsingStruct infoForParsing;

        infoForParsing.showStickers = true;
        infoForParsing.hideUglyImages = false;
        infoForParsing.stickerToSmiley = false;
        infoForParsing.smileyToText = true;
        infoForParsing.stickersSize = 68;
        infoForParsing.nbMaxQuote = 3;
        infoForParsing.betterQuote = false;
        infoForParsing.betterCodeTag = false;
        infoForParsing.noelshackImageWidth = 68;
        infoForParsing.noelshackImageHeight = 51;

        return infoForParsing;
    }

    void testPayloadDecoding()
    {
        testTool::startGroup("Décodage du payload");

        QString source = testTool::loadFixture("topic-page1.html.gz");

        testTool::checkTrue("la fixture se charge", source.size() > 100000);
        testTool::checkTrue("la page contient un payload", payloadTool::sourceContainsPayload(source));

        QJsonObject payload = payloadTool::getPayload(source);

        testTool::checkTrue("le payload se décode", payload.isEmpty() == false);
        testTool::checkTrue("le payload contient listMessage", payload.contains("listMessage"));

        QString pageWithoutPayload = testTool::loadFixture("topic-inexistant.html");

        testTool::checkTrue("une page de redirection n'a pas de payload", payloadTool::sourceContainsPayload(pageWithoutPayload) == false);
        testTool::checkTrue("le payload d'une page de redirection est vide", payloadTool::getPayload(pageWithoutPayload).isEmpty());
    }

    void testMessagesOfTopic()
    {
        testTool::startGroup("Messages d'un topic");

        QString source = testTool::loadFixture("topic-page1.html.gz");
        QList<messageStruct> listOfMessages = parsingTool::getListOfEntireMessagesWithoutMessagePars(source);

        testTool::checkEquals("nombre de messages", listOfMessages.size(), 20);

        if(listOfMessages.isEmpty() == true)
        {
            return;
        }

        const messageStruct& firstMessage = listOfMessages.first();

        testTool::checkEquals("id du premier message", firstMessage.idOfMessage, 1316659360L);
        testTool::checkEquals("pseudo du premier message", firstMessage.pseudoInfo.pseudoName, QString("Hillojow"));
        testTool::checkEquals("type de pseudo par défaut", firstMessage.pseudoInfo.pseudoType, QString("user"));
        testTool::checkEquals("date complète", firstMessage.wholeDate, QString("02 juin 2026 à 22:25:06"));
        testTool::checkEquals("heure seule", firstMessage.date, QString("22:25:06"));
        testTool::checkEquals("date d'édition", firstMessage.lastTimeEdit, QString("02 juin 2026 à 22:29:23"));
        testTool::checkEquals("heure d'édition", firstMessage.lastTimeEditHourOnly, QString("22:29:23"));
        testTool::checkEquals("avatar sans le schéma", firstMessage.avatarLink,
                              QString("image.jeuxvideo.com/avatar-md/h/i/hillojow-1740390047-5095618d3cfd5a0312d25e06374dd777.jpg"));
        testTool::checkTrue("le message n'est pas vide", firstMessage.message.isEmpty() == false);

        testTool::checkEquals("id du dernier message", listOfMessages.last().idOfMessage, 1316661488L);
        testTool::checkEquals("pseudo du dernier message", listOfMessages.last().pseudoInfo.pseudoName, QString("Treiben"));

        bool messagesSortedById = true;
        for(int i = 1; i < listOfMessages.size(); ++i)
        {
            if(listOfMessages[i - 1].idOfMessage > listOfMessages[i].idOfMessage)
            {
                messagesSortedById = false;
            }
        }
        testTool::checkTrue("les messages sont triés par id", messagesSortedById);

        bool allMessagesHaveAPseudo = true;
        for(const messageStruct& thisMessage : listOfMessages)
        {
            if(thisMessage.pseudoInfo.pseudoName.isEmpty() == true || thisMessage.idOfMessage == 0)
            {
                allMessagesHaveAPseudo = false;
            }
        }
        testTool::checkTrue("tous les messages ont un pseudo et un id", allMessagesHaveAPseudo);
    }

    void testInfosOfTopic()
    {
        testTool::startGroup("Infos d'un topic");

        QString source = testTool::loadFixture("topic-page1.html.gz");

        testTool::checkEquals("nom du topic", parsingTool::getNameOfTopic(source), QString("[Blabla] Le cadeau de l'ange"));
        testTool::checkEquals("nombre de connectés", parsingTool::getNumberOfConnected(source), QString("7"));
        testTool::checkEquals("topic non fermé", parsingTool::getTopicLocked(source), false);

        QList<QPair<QString, QString>> listOfInput;
        parsingTool::getListOfHiddenInputFromThisForm(source, "form-post-topic", listOfInput);

        /* Les quatre champs de `formSession`, plus le jeton ajax ajouté au passage. */
        testTool::checkEquals("champs cachés du formulaire", listOfInput.size(), 5);

        bool ajaxHashFound = false;
        for(const QPair<QString, QString>& thisInput : listOfInput)
        {
            if(thisInput.first == "ajax_hash" && thisInput.second.isEmpty() == false)
            {
                ajaxHashFound = true;
            }
        }
        testTool::checkTrue("ajax_hash est présent et rempli", ajaxHashFound);

        bool sessionFound = false;
        for(const QPair<QString, QString>& thisInput : listOfInput)
        {
            if(thisInput.first == "fs_session" && thisInput.second.isEmpty() == false)
            {
                sessionFound = true;
            }
        }
        testTool::checkTrue("fs_session est présent et rempli", sessionFound);

        ajaxInfoStruct ajaxInfo = parsingTool::getAjaxInfo(source);

        testTool::checkContains("ajaxInfo contient un hash", ajaxInfo.list, "ajax_hash=");
        testTool::checkDoesNotContain("ajaxInfo n'a pas de hash vide", ajaxInfo.list, "ajax_hash=&");
    }

    void testPagination()
    {
        testTool::startGroup("Pagination");

        QString sourceOfFirstPage = testTool::loadFixture("topic-page1.html.gz");
        pagerInfoStruct pagerInfo = parsingTool::getPagerInfo(sourceOfFirstPage, websiteOfJvc);

        testTool::checkEquals("page courante", pagerInfo.currentPage, 1);
        testTool::checkEquals("nombre de pages", pagerInfo.numberOfPages, 1295);

        testTool::checkEquals("page suivante depuis la page 1",
                              parsingTool::getNextPageOfTopic(sourceOfFirstPage, websiteOfJvc),
                              QString("https://www.jeuxvideo.com/forums/42-3016891-77054608-2-0-1-0-blabla-le-cadeau-de-l-ange.htm"));

        /* Le payload ne liste que quelques pages autour de la page courante : la
         * dernière page doit être reconstruite, pas cherchée dans la liste. */
        testTool::checkEquals("dernière page depuis la page 1",
                              parsingTool::getLastPageOfTopic(sourceOfFirstPage, websiteOfJvc),
                              QString("https://www.jeuxvideo.com/forums/42-3016891-77054608-1295-0-1-0-blabla-le-cadeau-de-l-ange.htm"));

        QString sourceOfDeepPage = testTool::loadFixture("topic-page1200.html.gz");

        testTool::checkEquals("page courante profonde", parsingTool::getPagerInfo(sourceOfDeepPage, websiteOfJvc).currentPage, 1200);
        testTool::checkEquals("page suivante depuis la page 1200",
                              parsingTool::getNextPageOfTopic(sourceOfDeepPage, websiteOfJvc),
                              QString("https://www.jeuxvideo.com/forums/42-3016891-77054608-1201-0-1-0-blabla-le-cadeau-de-l-ange.htm"));
        testTool::checkEquals("dernière page depuis la page 1200",
                              parsingTool::getLastPageOfTopic(sourceOfDeepPage, websiteOfJvc),
                              QString("https://www.jeuxvideo.com/forums/42-3016891-77054608-1295-0-1-0-blabla-le-cadeau-de-l-ange.htm"));

        testTool::checkEquals("première page d'un lien de topic",
                              parsingTool::getFirstPageOfTopic("https://www.jeuxvideo.com/forums/42-3016891-77054608-42-0-1-0-blabla-le-cadeau-de-l-ange.htm"),
                              QString("https://www.jeuxvideo.com/forums/42-3016891-77054608-1-0-1-0-blabla-le-cadeau-de-l-ange.htm"));
        testTool::checkEquals("page précédente d'un lien de topic",
                              parsingTool::getBeforeLastPageOfTopic("https://www.jeuxvideo.com/forums/42-3016891-77054608-42-0-1-0-blabla-le-cadeau-de-l-ange.htm"),
                              QString("https://www.jeuxvideo.com/forums/42-3016891-77054608-41-0-1-0-blabla-le-cadeau-de-l-ange.htm"));
    }

    void testListOfTopics()
    {
        testTool::startGroup("Liste des sujets d'un forum");

        QString source = testTool::loadFixture("forum.html.gz");
        QList<topicStruct> listOfTopic = parsingTool::getListOfTopic(source, websiteOfJvc);

        testTool::checkEquals("nombre de sujets", listOfTopic.size(), 25);

        if(listOfTopic.isEmpty() == true)
        {
            return;
        }

        testTool::checkEquals("nom du premier sujet", listOfTopic.first().name, QString("[Moderation] QG de l'Ordre de Favonius"));
        testTool::checkEquals("lien du premier sujet", listOfTopic.first().link,
                              QString("https://www.jeuxvideo.com/forums/42-3016891-64938461-1-0-1-0-moderation-qg-de-l-ordre-de-favonius.htm"));
        testTool::checkEquals("sujet épinglé", listOfTopic.first().topicType, QString("topic-pin-on"));
        testTool::checkEquals("auteur modérateur", listOfTopic.first().pseudoInfo.pseudoType, QString("modo"));
        testTool::checkEquals("nombre de messages", listOfTopic.first().numberOfMessage, QString("2830"));

        bool allTopicsHaveALink = true;
        for(const topicStruct& thisTopic : listOfTopic)
        {
            if(thisTopic.link.startsWith("https://www.jeuxvideo.com/forums/") == false || thisTopic.name.isEmpty() == true)
            {
                allTopicsHaveALink = false;
            }
        }
        testTool::checkTrue("tous les sujets ont un nom et un lien", allTopicsHaveALink);

        testTool::checkEquals("nom du forum", parsingTool::getForumName(source), QString("Forum Genshin Impact"));
    }

    void testConversionsOfState()
    {
        testTool::startGroup("Conversions d'état");

        testTool::checkEquals("rôle vide", parsingTool::roleToPseudoType(""), QString("user"));
        testTool::checkEquals("rôle modérateur", parsingTool::roleToPseudoType("moderator"), QString("modo"));
        testTool::checkEquals("rôle administrateur", parsingTool::roleToPseudoType("administrator"), QString("admin"));
        testTool::checkEquals("rôle inconnu", parsingTool::roleToPseudoType("nimportequoi"), QString("user"));

        testTool::checkEquals("sujet supprimé", parsingTool::stateOfTopicToTopicType("msg-deleted", "normal"), QString("topic-removed"));
        testTool::checkEquals("sujet épinglé fermé", parsingTool::stateOfTopicToTopicType("msg-visible", "pinned-off"), QString("topic-pin-off"));
        testTool::checkEquals("sujet actif", parsingTool::stateOfTopicToTopicType("msg-visible", "hot"), QString("topic-folder2"));
        testTool::checkEquals("sujet normal", parsingTool::stateOfTopicToTopicType("msg-visible", "normal"), QString("topic-folder"));

        testTool::checkEquals("heure d'une date", parsingTool::getHourOfDate("02 juin 2026 à 22:25:06"), QString("22:25:06"));
        testTool::checkEquals("heure d'une date sans heure", parsingTool::getHourOfDate(""), QString(""));
        testTool::checkEquals("url sans schéma", parsingTool::removeSchemeOfUrl("https://exemple.fr/a.jpg"), QString("exemple.fr/a.jpg"));
        testTool::checkEquals("url déjà sans schéma", parsingTool::removeSchemeOfUrl("exemple.fr/a.jpg"), QString("exemple.fr/a.jpg"));
    }

    void testContentOfMessages()
    {
        testTool::startGroup("Contenu des messages");

        QString source = testTool::loadFixture("topic-page1.html.gz");
        QList<messageStruct> listOfMessages = parsingTool::getListOfEntireMessagesWithoutMessagePars(source);

        if(listOfMessages.isEmpty() == true)
        {
            testTool::reportFailure("des messages à analyser", "au moins un message", "aucun");
            return;
        }

        infoForMessageParsingStruct infoForParsing = makeBaseInfoForParsing();
        QStringList listOfNoelshackImages;
        infoForParsing.listOfNoelshackImageUsed = &listOfNoelshackImages;

        QString messageWithSpoil;
        QString messageWithSmiley;
        QString messageWithQuote;

        for(const messageStruct& thisMessage : listOfMessages)
        {
            QString parsedMessage = parsingTool::parsingMessages(thisMessage.message, infoForParsing);

            if(messageWithSpoil.isEmpty() == true && thisMessage.message.contains("message__spoil") == true)
            {
                messageWithSpoil = parsedMessage;
            }
            if(messageWithSmiley.isEmpty() == true && thisMessage.message.contains("message__smiley") == true)
            {
                messageWithSmiley = parsedMessage;
            }
            if(messageWithQuote.isEmpty() == true && thisMessage.message.contains("<blockquote>") == true)
            {
                messageWithQuote = parsedMessage;
            }

            testTool::checkDoesNotContain("pas de classe message__ résiduelle dans le message " +
                                          QString::number(thisMessage.idOfMessage), parsedMessage, "message__");
        }

        testTool::checkTrue("des images noelshack ont été trouvées", listOfNoelshackImages.isEmpty() == false);

        bool allNoelshackImagesHaveAScheme = true;
        for(const QString& thisImage : listOfNoelshackImages)
        {
            if(thisImage.startsWith("https://") == false && thisImage.startsWith("http://") == false)
            {
                allNoelshackImagesHaveAScheme = false;
            }
        }
        testTool::checkTrue("les images noelshack ont un schéma", allNoelshackImagesHaveAScheme);

        testTool::checkTrue("un message contenait un spoil", messageWithSpoil.isEmpty() == false);
        if(messageWithSpoil.isEmpty() == false)
        {
            testTool::checkContains("le spoil est masqué par une couleur", messageWithSpoil, "background-color");
            testTool::checkDoesNotContain("l'habillage du spoil a disparu", messageWithSpoil, "Afficher");
        }

        testTool::checkTrue("un message contenait un smiley", messageWithSmiley.isEmpty() == false);
        if(messageWithSmiley.isEmpty() == false)
        {
            testTool::checkDoesNotContain("le smiley n'est plus une balise img brute", messageWithSmiley, "smileys_img");
        }

        testTool::checkTrue("un message contenait une citation", messageWithQuote.isEmpty() == false);
        if(messageWithQuote.isEmpty() == false)
        {
            testTool::checkContains("la citation est devenue un tableau", messageWithQuote, "<table");
        }

        /* Les paragraphes doivent être convertis en sauts de ligne, pas laissés tels quels. */
        QString parsedFirstMessage = parsingTool::parsingMessages(listOfMessages.first().message, infoForParsing);
        testTool::checkDoesNotContain("plus de balise <p> ouvrante", parsedFirstMessage, "<p ");
        testTool::checkDoesNotContain("plus de balise </p>", parsedFirstMessage, "</p>");
    }

    void testQuoting()
    {
        testTool::startGroup("Citation d'un message");

        QString source = testTool::loadFixture("topic-page1.html.gz");
        QList<messageStruct> listOfMessages = parsingTool::getListOfEntireMessagesWithoutMessagePars(source);

        testTool::checkTrue("le texte source des messages est conservé",
                            listOfMessages.isEmpty() == false && listOfMessages.first().messageRaw.isEmpty() == false);

        testTool::checkEquals("chaque ligne est préfixée",
                              parsingTool::quoteThisRawMessage("une ligne\nune autre"),
                              QString("une ligne\n>une autre"));

        /* Au-delà de quatre niveaux d'imbrication la citation devient illisible, JVC
         * refuse d'ailleurs les messages trop longs. */
        testTool::checkEquals("les citations trop imbriquées sont tronquées",
                              parsingTool::quoteThisRawMessage("> > > > > perdu au fond\n> > > > gardé\nfin"),
                              QString("> > > > ''[citation imbriquée tronquée]''\n>> > > > gardé\n>fin"));
        testTool::checkEquals("plusieurs lignes trop imbriquées sont regroupées",
                              parsingTool::quoteThisRawMessage(">>>>>a\n>>>>>b\nfin"),
                              QString("> > > > ''[citation imbriquée tronquée]''\n>fin"));
        testTool::checkEquals("une citation normale est intacte",
                              parsingTool::quoteThisRawMessage("> > citation\nréponse"),
                              QString("> > citation\n>réponse"));
    }

    void testMultipartFormData()
    {
        testTool::startGroup("Formulaire multipart");

        QList<QPair<QString, QString>> listOfField;
        listOfField.append(QPair<QString, QString>("text", "coucou"));
        listOfField.append(QPair<QString, QString>("topicId", "77054608"));

        QByteArray boundaryUsed;
        QString body = QString::fromUtf8(parsingTool::buildMultipartFormData(listOfField, boundaryUsed));

        testTool::checkTrue("une frontière est générée", boundaryUsed.isEmpty() == false);
        testTool::checkContains("le champ texte est présent", body, "name=\"text\"\r\n\r\ncoucou\r\n");
        testTool::checkContains("le champ topicId est présent", body, "name=\"topicId\"\r\n\r\n77054608\r\n");
        testTool::checkContains("le corps est correctement terminé", body, "--" + QString::fromUtf8(boundaryUsed) + "--\r\n");
        testTool::checkTrue("la frontière n'apparaît pas dans les valeurs",
                            body.count(QString::fromUtf8(boundaryUsed)) == 3);

        testTool::checkEquals("identifiant de forum d'un lien de topic",
                              parsingTool::getForumIdOfThisTopic("https://www.jeuxvideo.com/forums/42-3016891-77054608-1-0-1-0-blabla.htm"),
                              QString("3016891"));
        testTool::checkEquals("identifiant de topic d'un lien de topic",
                              parsingTool::getTopicIdOfThisTopic("https://www.jeuxvideo.com/forums/42-3016891-77054608-1-0-1-0-blabla.htm"),
                              QString("77054608"));
    }

    void testErrorMessages()
    {
        testTool::startGroup("Messages d'erreur");

        testTool::checkEquals("erreur JSON au format tableau",
                              parsingTool::getErrorMessageInJSON(R"({"erreur":["Vous postez trop vite."]})", false, "défaut"),
                              QString("Vous postez trop vite."));
        testTool::checkEquals("erreur JSON au format chaîne",
                              parsingTool::getErrorMessageInJSON(R"({"errors":"Message trop long."})", false, "défaut"),
                              QString("Message trop long."));
        testTool::checkEquals("pas d'erreur",
                              parsingTool::getErrorMessageInJSON(R"({"erreur":[]})", false, "défaut"),
                              QString("défaut"));
    }

    void testVerdictOfMessageSending()
    {
        testTool::startGroup("Verdict d'envoi d'un message");

        /* Le bug corrigé : une réponse JSON de succès était prise pour un échec, et le
         * programme annonçait « le message n'a pas été envoyé » sur un message posté. */
        testTool::checkEquals("liste d'erreurs vide = succès",
                              parsingTool::getErrorOfMessageSending(R"({"erreur":[]})", 200), QString(""));
        testTool::checkEquals("réponse de succès inconnue = succès",
                              parsingTool::getErrorOfMessageSending(R"({"success":true,"id":123})", 200), QString(""));
        testTool::checkEquals("message rendu renvoyé = succès",
                              parsingTool::getErrorOfMessageSending(R"({"message":{"id":1,"renderedText":"<p>salut</p>"}})", 200), QString(""));
        testTool::checkEquals("objet vide = succès",
                              parsingTool::getErrorOfMessageSending("{}", 200), QString(""));
        testTool::checkEquals("corps vide = succès",
                              parsingTool::getErrorOfMessageSending("", 200), QString(""));
        testTool::checkEquals("redirection = succès",
                              parsingTool::getErrorOfMessageSending(R"(<meta http-equiv="refresh" content="0">)", 200), QString(""));

        testTool::checkEquals("erreur en tableau",
                              parsingTool::getErrorOfMessageSending(R"({"erreur":["Vous postez trop vite."]})", 200),
                              QString("Vous postez trop vite."));
        testTool::checkEquals("erreur en chaîne",
                              parsingTool::getErrorOfMessageSending(R"({"errors":"Session expirée."})", 403),
                              QString("Session expirée."));
        testTool::checkContains("erreur échappée en JSON",
                              parsingTool::getErrorOfMessageSending(R"({"erreur":["Message trop long \/ invalide"]})", 200),
                              "Message trop long / invalide");

        /* Un code d'erreur sans message reste une erreur, sinon on annoncerait un
         * succès sur un refus silencieux. */
        testTool::checkContains("échec HTTP sans message",
                              parsingTool::getErrorOfMessageSending("{}", 500), "500");
        testTool::checkEquals("succès HTTP sans message",
                              parsingTool::getErrorOfMessageSending("{}", 201), QString(""));
    }

    void testActionsOnMessages()
    {
        testTool::startGroup("Actions permises sur un message");

        /* Fixture fabriquée à la main, calquée sur une vraie page connectée : on ne peut
         * pas figer une page connectée réelle dans le dépôt, elle contient des jetons de
         * session. */
        QString source = testTool::loadFixture("topic-connecte.html.gz");
        QList<messageStruct> listOfMessages = parsingTool::getListOfEntireMessagesWithoutMessagePars(source);

        testTool::checkEquals("nombre de messages", listOfMessages.size(), 3);

        if(listOfMessages.size() < 3)
        {
            return;
        }

        testTool::checkEquals("aucune action sur le message d'un autre", listOfMessages[0].editUrl, QString(""));
        testTool::checkEquals("pas de suppression sur le message d'un autre", listOfMessages[0].deleteUrl, QString(""));

        /* Le cas qui faisait échouer l'édition : le message est à nous, mais le site n'en
         * propose plus la modification. */
        testTool::checkEquals("message à nous mais non modifiable", listOfMessages[1].editUrl, QString(""));
        testTool::checkContains("message à nous supprimable", listOfMessages[1].deleteUrl,
                                "https://www.jeuxvideo.com/forums/message/delete?ids=1000000002");

        testTool::checkContains("message à nous modifiable", listOfMessages[2].editUrl,
                                "https://www.jeuxvideo.com/forums/message/edit?id=1000000003");
        testTool::checkContains("l'URL d'action garde son jeton", listOfMessages[2].editUrl, "ajax_hash=");

        testTool::checkEquals("une URL déjà absolue n'est pas préfixée",
                              parsingTool::makeAbsoluteUrl("https://www.forumjv.com/a", "www.jeuxvideo.com"),
                              QString("https://www.forumjv.com/a"));
        testTool::checkEquals("une URL relative prend le domaine du payload",
                              parsingTool::makeAbsoluteUrl("/forums/message/delete", "www.forumjv.com"),
                              QString("https://www.forumjv.com/forums/message/delete"));
        testTool::checkEquals("une URL vide reste vide",
                              parsingTool::makeAbsoluteUrl("", "www.jeuxvideo.com"), QString(""));

        /* Déconnecté, le site ne donne aucune action : rien ne doit être proposé. */
        QList<messageStruct> listOfMessagesLoggedOut =
            parsingTool::getListOfEntireMessagesWithoutMessagePars(testTool::loadFixture("topic-page1.html.gz"));

        bool noActionWhenLoggedOut = true;
        for(const messageStruct& thisMessage : listOfMessagesLoggedOut)
        {
            if(thisMessage.editUrl.isEmpty() == false || thisMessage.deleteUrl.isEmpty() == false)
            {
                noActionWhenLoggedOut = false;
            }
        }
        testTool::checkTrue("aucune action quand on n'est pas connecté", noActionWhenLoggedOut);
    }

    void testEditFormValues()
    {
        testTool::startGroup("Valeurs du formulaire d'édition");

        /* Réponse réelle de /forums/message/edit/form-values, jetons remplacés. */
        QString source = R"({"formSession":{"fs_session":"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",)"
                         R"("fs_timestamp":1785159300,"fs_version":"forum_edit_message",)"
                         R"("fs_bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb":"cccccccccccccccccccccccccccccccccccccccc"},)"
                         R"("needsCaptcha":false,"text":"Ça va !","ajaxToken":"dddddddddddddddddddddddddddddddddddddddd"})";

        editFormValuesStruct formValues = parsingTool::getEditFormValues(source);

        testTool::checkTrue("les valeurs sont exploitables", formValues.isValid);
        testTool::checkEquals("le texte à éditer", formValues.text, QString("Ça va !"));
        testTool::checkEquals("pas de captcha", formValues.needsCaptcha, false);
        /* Les quatre champs de formSession, plus ajax_hash. */
        testTool::checkEquals("nombre de champs à renvoyer", formValues.listOfField.size(), 5);

        QString versionOfForm;
        QString ajaxHash;
        for(const QPair<QString, QString>& thisField : formValues.listOfField)
        {
            if(thisField.first == "fs_version") { versionOfForm = thisField.second; }
            if(thisField.first == "ajax_hash") { ajaxHash = thisField.second; }
        }

        /* La session d'édition n'est pas celle de la page : c'est ce qui manquait pour
         * que le site accepte la modification. */
        testTool::checkEquals("la session est celle de l'édition", versionOfForm, QString("forum_edit_message"));
        testTool::checkEquals("le jeton ajax est repris", ajaxHash, QString("dddddddddddddddddddddddddddddddddddddddd"));

        testTool::checkTrue("une réponse vide est rejetée", parsingTool::getEditFormValues("").isValid == false);
        testTool::checkTrue("une page HTML est rejetée", parsingTool::getEditFormValues("<html>404</html>").isValid == false);
        testTool::checkTrue("un JSON sans formSession est rejeté", parsingTool::getEditFormValues(R"({"text":"a"})").isValid == false);

        /* Réponse réelle à l'envoi d'une modification acceptée. */
        testTool::checkEquals("modification acceptée",
                              parsingTool::getErrorOfMessageSending(R"({"html":"<p>ok<\/p>","formSession":{"fs_timestamp":0}})", 200),
                              QString(""));
    }

    void testPageWithoutMessages()
    {
        testTool::startGroup("Page sans message");

        QString source = testTool::loadFixture("topic-inexistant.html");

        testTool::checkEquals("aucun message", parsingTool::getListOfEntireMessagesWithoutMessagePars(source).size(), 0);
        testTool::checkEquals("aucun sujet", parsingTool::getListOfTopic(source, websiteOfJvc).size(), 0);
        testTool::checkEquals("pas de nom de topic", parsingTool::getNameOfTopic(source), QString(""));
        testTool::checkEquals("pas de page suivante", parsingTool::getNextPageOfTopic(source, websiteOfJvc), QString(""));

        /* Une page de forum n'est pas une page de topic : il ne doit pas en sortir de messages. */
        QString sourceOfForum = testTool::loadFixture("forum.html.gz");
        testTool::checkEquals("une page de forum ne donne pas de messages",
                              parsingTool::getListOfEntireMessagesWithoutMessagePars(sourceOfForum).size(), 0);
    }
}

void runParsingTests()
{
    testPayloadDecoding();
    testMessagesOfTopic();
    testInfosOfTopic();
    testPagination();
    testListOfTopics();
    testConversionsOfState();
    testContentOfMessages();
    testQuoting();
    testMultipartFormData();
    testErrorMessages();
    testVerdictOfMessageSending();
    testActionsOnMessages();
    testEditFormValues();
    testPageWithoutMessages();
}
