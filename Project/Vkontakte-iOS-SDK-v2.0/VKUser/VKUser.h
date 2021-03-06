//
// Created by AndrewShmig on 6/28/13.
//
// Copyright (c) 2013 Andrew Shmig
// 
// Permission is hereby granted, free of charge, to any person 
// obtaining a copy of this software and associated documentation 
// files (the "Software"), to deal in the Software without 
// restriction, including without limitation the rights to use, 
// copy, modify, merge, publish, distribute, sublicense, and/or 
// sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following 
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
// FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
// THE SOFTWARE.
//
#import <Foundation/Foundation.h>
#import "VKCachedData.h"


@class VKAccessToken;
@class VKRequest;
@protocol VKRequestDelegate;


/**
 Класс представляет пользователя социальной сети, который может осуществлять ряд
 действий с объектами вроде групп, друзей, музыкой, видео и тд.

 Класс так же позволяет изменить текущего активного пользователя на одного из
 пользователей находящихся в хранилище (ранее авторизованных). Получить список
 пользователей, которые авторизовывались можно используя метод localUsers.

 @warning методы, которые требуют наличия access_token в запросе _перезаписывают_
 токен доступа, если таковой был указан при передаче словаря ключей-значений, на
 значение токена доступа текущего пользователя - self.accessToken.token.

 @warning по умолчанию у каждого запроса из класса VKUser подпись (signature)
 является строкой селектора инициировавшего/создавшего объекта запроса
 */
@interface VKUser : NSObject

/**
@name Свойства
*/
/** Делегат

Решение использовать сильную ссылку на объект делегата в классе VKUser возникает
при рассмотрении детально принципа работы запросов VKRequest.
В VKRequest ссылка на делегат является слабой, а значит, если и в VKUser будет
слабой, то вполне может случиться неприятность, когда будут запущено несколько
запросов, а делегат в некоторый момент времени установлен в nil (случайно или
намеренно), тогда получим пустые запросы результаты которых не нужны будут и не
будут использованы.

@warning возможно в будущем изменится обработка момента присвоения делегату
значения nil, как вариант, все запущенные запросы будут отменены.
*/
@property (nonatomic, strong, readwrite) id<VKRequestDelegate> delegate;

/** Пользовательски токен доступа текущего активного пользователя
*/
@property (nonatomic, readonly) VKAccessToken *accessToken;

/** Начинать ли выполнение запросов немедленно или предоставить программисту
самому выбирать момент запуска запроса.
По умолчанию принимает значение YES.

Предположим, что вы хотите осуществить запрос пользовательской информации, но
начало хотите инициировать сами. Вот, как это может выглядеть:

    [VKUser currentUser].startAllRequestsImmediately = NO;
    VKRequest *userInfo = [[VKUser currentUser] info];

    // пользователь нажал какую-то кнопку, после чего вы стартуете запрос
    [userInfo start];

Если нет необходимоти выполнять отложенный запуск, то можно делать следующим образом:

    // запрос стартует немедленно
    [[VKUser currentUser] info];

*/
@property (nonatomic, assign, readwrite) BOOL startAllRequestsImmediately;

/** Оффлайн режим. В данном режиме данные будут запрошены из кэша и возвращены
даже в случае истечения срока их действия (удаления не произойдет).
По умолчанию режим выключен.
*/
@property (nonatomic, assign, readwrite) BOOL offlineMode;

/**
@name Методы класса
*/
/** Текущий активный пользователь.

Если хранилище не содержит авторизованных пользователей, то возвращено будет значение
nil.

В случае, если активным не был установлен какой-то определенный пользователь, то
при вызове данного метода активируется произвольный пользователь из хранилища (если
в хранилище будет находится лишь один пользователь, то именно он будет активирован
и от его лица будут осуществляться дальнейшие запросы).

_Вопрос:_ Когда может произойти подобная ситуация?

_Ответ:_ Представим, что два и более пользователей подряд авторизовались и, во время
авторизаций не было вызовов метода currentUser.

Рассмотрим на примере:

    // авторизовался пользователь А
    // авторизовался пользователь Б
    // авторизовался пользователь В
    [VKUser currentUser] // будет активирован произвольный пользователь, либо А, либо Б, либо В

Второй пример:

    // авторизовался пользователь А
    [VKUser currentUser] // активным устанавливается пользователь А
    // авторизовался пользователь Б, но А по прежнему активный
    // авторизовался пользователь В, но А по прежнему активный

*/
+ (instancetype)currentUser;

/** Делает активным пользователя с идентификатором userID.

Если пользователя с таким идентификатором нет в хранилище, то метод вернет NO, иначе
YES.

@param userID идентификатор пользователя, которого необходимо активировать

@return булево значение, удалось ли активировать указанного пользователя или нет
*/
+ (BOOL)activateUserWithID:(NSUInteger)userID;

/** Получение списка пользователей находящихся в хранилище

@return массив пользовательских идентификаторов
*/
+ (NSArray *)localUsers;

/**
@name Пользователи
*/
/** Информация о текущем пользователе.

Следующие поля данных запрашиваются: nickname,screen_name,sex,bdate,has_mobile,online,last_seen,status,photo100

Детальную информацию можно найти по этой ссылке в документации: https://vk.com/dev/users.get

@return экземпляр класса VKRequest, который инкапсулирует в себе все необходимые параметры для
осуществления запроса пользовательской информации. Объект запроса возвращается по причине
возможной необходимости отменить выполнение текущего запроса или отложенное выполнение запроса.
*/
- (VKRequest *)info;

/** Информация о пользователе(ях) с указанными параметрами

Описание параметров и возможные значения можно найти по ссылке: https://vk.com/dev/users.get

@param options ключи-значения, которые будут переданы при запросе методом GET
@return @see info
*/
- (VKRequest *)infoWithCustomOptions:(NSDictionary *)options;

/** Возвращает список пользователей в соответствии с заданным критерием поиска

@param options ключи-значения, с полным списком ключей можно ознакомиться по
ссылке из документации: https://vk.com/dev/users.search
@return @see info
*/
- (VKRequest *)searchWithCustomOptions:(NSDictionary *)options;

/** Возвращает список идентификаторов пользователей и групп, которые входят в список подписок пользователя

@param options ключи-значения, с полным списком ключей можно ознакомиться по
ссылке из документации: https://vk.com/dev/users.getSubscriptions
@return @see info
*/
- (VKRequest *)subscriptionsWithCustomOptions:(NSDictionary *)options;

/** Возвращает список идентификаторов пользователей, которые являются подписчиками пользователя. Идентификаторы пользователей в списке отсортированы в порядке убывания времени их добавления.

@param options ключи-значения, с полным списком ключей можно ознакомиться по
ссылке из документации: https://vk.com/dev/users.getFollowers
@return @see info
*/
- (VKRequest *)followersWithCustomOptions:(NSDictionary *)options;

/**
@name Переопределенные методы
 */
/** Описание текущего пользователя
*/
- (NSString *)description;

@end

@interface VKUser (Wall)

/**
@name Стена
*/
/** Возвращает список записей со стены пользователя или сообщества

@param options ключи-значения, полный список в документации: https://vk.com/dev/wall.get
@return @see info
*/
- (VKRequest *)wallGetWithCustomOptions:(NSDictionary *)options;

/** Возвращает список записей со стен пользователей по их идентификаторам

@param options ключи-значения, полный список в документации: https://vk.com/dev/wall.getById
@return @see info
*/
- (VKRequest *)wallGetByIDWithCustomOptions:(NSDictionary *)options;

/** Сохраняет запись на стене пользователя. Запись может содержать фотографию, ранее загруженную на сервер ВКонтакте, или любую доступную фотографию из альбома пользователя.
При запуске со стены приложение открывается в окне размером 607x412 и ему передаются параметры, описанные здесь.

@param options ключи-значения, полный список в документации: https://vk.com/dev/wall.savePost
@return @see info
*/
- (VKRequest *)wallSavePostWithCustomOptions:(NSDictionary *)options;

/** Публикует новую запись на своей или чужой стене.
Данный метод позволяет создать новую запись на стене, а также опубликовать предложенную новость или отложенную запись.

@param options ключи-значения, полный список в документации: https://vk.com/dev/wall.post
@return @see info
*/
- (VKRequest *)wallPostWithCustomOptions:(NSDictionary *)options;

/** Копирует объект на стену пользователя или сообщества

@param options ключи-значения, полный список в документации: https://vk.com/dev/wall.repost
@return @see info
*/
- (VKRequest *)wallRepostWithCustomOptions:(NSDictionary *)options;

/** Позволяет получать список репостов заданной записи

@param options ключи-значения, полный список в документации: https://vk.com/dev/wall.getReposts
@return @see info
*/
- (VKRequest *)wallGetRepostsWithCustomOptions:(NSDictionary *)options;

/** Редактирует запись на стене

@param options ключи-значения, полный список в документации: https://vk.com/dev/wall.edit
@return @see info
*/
- (VKRequest *)wallEditWithCustomOptions:(NSDictionary *)options;

/** Удаляет запись со стены

@param options ключи-значения, полный список в документации: https://vk.com/dev/wall.delete
@return @see info
*/
- (VKRequest *)wallDeleteWithCustomOptions:(NSDictionary *)options;

/** Восстанавливает удаленную запись на стене пользователя

@param options ключи-значения, полный список в документации: https://vk.com/dev/wall.restore
@return @see info
*/
- (VKRequest *)wallRestoreWithCustomOptions:(NSDictionary *)options;

/** Возвращает список комментариев к записи на стене пользователя

@param options ключи-значения, полный список в документации: https://vk.com/dev/wall.getComments
@return @see info
*/
- (VKRequest *)wallGetCommentsWithCustomOptions:(NSDictionary *)options;

/** Добавляет комментарий к записи на стене пользователя или сообщества

@param options ключи-значения, полный список в документации: http://vk.com/dev/wall.addComment
@return @see info
*/
- (VKRequest *)wallAddCommentWithCustomOptions:(NSDictionary *)options;

/** Удаляет комментарий текущего пользователя к записи на своей или чужой стене

@param options ключи-значения, полный список в документации: https://vk.com/dev/wall.deleteComment
@return @see info
*/
- (VKRequest *)wallDeleteCommentWithCustomOptions:(NSDictionary *)options;

/** Восстанавливает комментарий текущего пользователя к записи на своей или чужой стене

@param options ключи-значения, полный список в документации: https://vk.com/dev/wall.restoreComment
@return @see info
*/
- (VKRequest *)wallRestoreCommentWithCustomOptions:(NSDictionary *)options;

@end

@interface VKUser (Photos)

/**
@name Фотографии
*/
/** Создает пустой альбом для фотографий

@param options ключи-значения, полный список по ссылке: https://vk.com/dev/photos.createAlbum
@return @see info
*/
- (VKRequest *)photosCreateAlbumWithCustomOptions:(NSDictionary *)options;

/** Редактирует данные альбома для фотографий пользователя

@param options ключи-значения, полный список по ссылке: https://vk.com/dev/photos.editAlbum
@return @see info
*/
- (VKRequest *)photosEditAlbumWithCustomOptions:(NSDictionary *)options;

/** Возвращает список альбомов пользователя или сообщества

@param options ключи-значения, полный список по ссылке: https://vk.com/dev/photos.getAlbums
@return @see info
*/
- (VKRequest *)photosGetAlbumsWithCustomOptions:(NSDictionary *)options;

/** Возвращает список фотографий в альбоме

@param options ключи-значения, полный список по ссылке: https://vk.com/dev/photos.get
@return @see info
*/
- (VKRequest *)photosGetWithCustomOptions:(NSDictionary *)options;

/** Возвращает количество доступных альбомов пользователя

@param options ключи-значения, полный список по ссылке: https://vk.com/dev/photos.getAlbumsCount
@return @see info
*/
- (VKRequest *)photosGetAlbumsCountWithCustomOptions:(NSDictionary *)options;

/** Возвращает список фотографий со страницы пользователя или сообщества

@param options ключи-значения, полный список по ссылке: https://vk.com/dev/photos.getProfile
@return @see info
*/
- (VKRequest *)photosGetProfileWithCustomOptions:(NSDictionary *)options;

/** Возвращает информацию о фотографиях по их идентификаторам

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/photos.getById
@return @see info
*/
- (VKRequest *)photosGetByIDWithCustomOptions:(NSDictionary *)options;

/** Возвращает адрес сервера для загрузки фотографий

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.getUploadServer
@return @see info
*/
- (VKRequest *)photosGetUploadServerWithCustomOptions:(NSDictionary *)options;

/** Возвращает адрес сервера для загрузки фотографии на страницу пользователя

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.getProfileUploadServer
@return @see info
*/
- (VKRequest *)photosGetProfileUploadServerWithCustomOptions:(NSDictionary *)options;

/** Позволяет получить адрес для загрузки фотографий мультидиалогов

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.getChatUploadServer
@return @see info
*/
- (VKRequest *)photosGetChatUploadServerWithCustomOptions:(NSDictionary *)options;

/** Сохраняет фотографию пользователя после успешной загрузки

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.saveProfilePhoto
@return @see info
*/
- (VKRequest *)photosSaveProfilePhotoWithCustomOptions:(NSDictionary *)options;

/** Сохраняет фотографии после успешной загрузки на URI, полученный методом photos.getWallUploadServer

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.saveWallPhoto
@return @see info
*/
- (VKRequest *)photosSaveWallPhotoWithCustomOptions:(NSDictionary *)options;

/** Возвращает адрес сервера для загрузки фотографии на стену пользователя.

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.getWallUploadServer
@return @see info
*/
- (VKRequest *)photosGetWallUploadServerWithCustomOptions:(NSDictionary *)options;

/** Возвращает адрес сервера для загрузки фотографии в личное сообщение пользователю

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.getMessagesUploadServer
@return @see info
*/
- (VKRequest *)photosGetMessagesUploadServerWithCustomOptions:(NSDictionary *)options;

/** Сохраняет фотографию после успешной загрузки на URI, полученный методом photos.getMessagesUploadServer

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.saveMessagesPhoto
@return @see info
*/
- (VKRequest *)photosSaveMessagesPhotoWithCustomOptions:(NSDictionary *)options;

/** Осуществляет поиск изображений по местоположению или описанию

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.search
@return @see info
*/
- (VKRequest *)photosSearchWithCustomOptions:(NSDictionary *)options;

/** Сохраняет фотографии после успешной загрузки

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.save
@return @see info
*/
- (VKRequest *)photosSaveWithCustomOptions:(NSDictionary *)options;

/** Изменяет описание у выбранной фотографии

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.edit
@return @see info
*/
- (VKRequest *)photosEditWithCustomOptions:(NSDictionary *)options;

/** Переносит фотографию из одного альбома в другой

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.move
@return @see info
*/
- (VKRequest *)photosMoveWithCustomOptions:(NSDictionary *)options;

/** Делает фотографию обложкой альбома

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.makeCover
@return @see info
*/
- (VKRequest *)photosMakeCoverWithCustomOptions:(NSDictionary *)options;

/** Меняет порядок альбома в списке альбомов пользователя

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.reorderAlbums
@return @see info
*/
- (VKRequest *)photosReorderAlbumsWithCustomOptions:(NSDictionary *)options;

/** Меняет порядок фотографии в списке фотографий альбома пользователя

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.reorderPhotos
@return @see info
*/
- (VKRequest *)photosReorderPhotosWithCustomOptions:(NSDictionary *)options;

/** Возвращает все фотографии пользователя или сообщества в антихронологическом порядке

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.getAll
@return @see info
*/
- (VKRequest *)photosGetAllWithCustomOptions:(NSDictionary *)options;

/** Возвращает список фотографий, на которых отмечен пользователь

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.getUserPhotos
@return @see info
*/
- (VKRequest *)photosGetUserPhotosWithCustomOptions:(NSDictionary *)options;

/** Удаляет указанный альбом для фотографий у текущего пользователя

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.deleteAlbum
@return @see info
*/
- (VKRequest *)photosDeleteAlbumWithCustomOptions:(NSDictionary *)options;

/** Удаление фотографии на сайте

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.delete
@return @see info
*/
- (VKRequest *)photosDeleteWithCustomOptions:(NSDictionary *)options;

/** Подтверждает отметку на фотографии

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.confirmTag
@return @see info
*/
- (VKRequest *)photosConfirmTagWithCusomOptions:(NSDictionary *)options;

/** Возвращает список комментариев к фотографии

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.getComments
@return @see info
*/
- (VKRequest *)photosGetCommentsWithCustomOptions:(NSDictionary *)options;

/** Возвращает отсортированный в антихронологическом порядке список всех комментариев к конкретному альбому или ко всем альбомам пользователя

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.getAllComments
@return @see info
*/
- (VKRequest *)photosGetAllCommentsWithCustomOptions:(NSDictionary *)options;

/** Создает новый комментарий к фотографии

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.createComment
@return @see info
*/
- (VKRequest *)photosCreateCommentWithCustomOptions:(NSDictionary *)options;

/** Удаляет комментарий к фотографии

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.deleteComment
@return @see info
*/
- (VKRequest *)photosDeleteCommentWithCustomOptions:(NSDictionary *)options;

/** Восстанавливает удаленный комментарий к фотографии

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.restoreComment
@return @see info
*/
- (VKRequest *)photosRestoreCommentWithCustomOptions:(NSDictionary *)options;

/** Изменяет текст комментария к фотографии

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.editComment
@return @see info
*/
- (VKRequest *)photosEditCommentWithCustomOptions:(NSDictionary *)options;

/** Возвращает список отметок на фотографии

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.getTags
@return @see info
*/
- (VKRequest *)photosGetTagsWithCustomOptions:(NSDictionary *)options;

/** Добавляет отметку на фотографию

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.putTag
@return @see info
*/
- (VKRequest *)photosPutTagWithCustomOptions:(NSDictionary *)options;

/** Удаляет отметку с фотографии

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.removeTag
@return @see info
*/
- (VKRequest *)photosRemoveTagWithCustomOptions:(NSDictionary *)options;

/** Возвращает список фотографий, на которых есть непросмотренные отметки

@param options ключи-значения, полный список здесь: https://vk.com/dev/photos.getNewTags
@return @see info
*/
- (VKRequest *)photosGetNewTagsWithCustomOptions:(NSDictionary *)options;

@end

@interface VKUser (Friends)

/**
@name Друзья
*/
/** Возвращает список идентификаторов друзей пользователя или расширенную информацию о друзьях пользователя (при использовании параметра fields)

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.get
@return @see info
*/
- (VKRequest *)friendsGetWithCustomOptions:(NSDictionary *)options;

/** Возвращает список идентификаторов друзей пользователя, находящихся на сайте

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.getOnline
@return @see info
*/
- (VKRequest *)friendsGetOnlineWithCustomOptions:(NSDictionary *)options;

/** Возвращает список идентификаторов общих друзей между парой пользователей

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.getMutual
@return @see info
*/
- (VKRequest *)friendsGetMutualWithCustomOptions:(NSDictionary *)options;

/** Возвращает список идентификаторов недавно добавленных друзей текущего пользователя

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.getRecent
@return @see info
*/
- (VKRequest *)friendsGetRecentWithCustomOptions:(NSDictionary *)options;

/** Возвращает информацию о полученных или отправленных заявках на добавление в друзья для текущего пользователя

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.getRequests
@return @see info
*/
- (VKRequest *)friendsGetRequestsWithCustomOptions:(NSDictionary *)options;

/** Одобряет или создает заявку на добавление в друзья.

Если идентификатор выбранного пользователя присутствует в списке заявок на добавление в друзья, полученном методом friends.getRequests, то одобряет заявку на добавление и добавляет выбранного пользователя в друзья к текущему пользователю. В противном случае создает заявку на добавление в друзья текущего пользователя к выбранному пользователю.

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.add
@return @see info
*/
- (VKRequest *)friendsAddWithCustomOptions:(NSDictionary *)options;

/** Редактирует списки друзей для выбранного друга

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.edit
@return @see info
*/
- (VKRequest *)friendsEditWithCustomOptions:(NSDictionary *)options;

/** Удаляет пользователя из списка друзей или отклоняет заявку в друзья

Если идентификатор выбранного пользователя присутствует в списке заявок на добавление в друзья, полученном методом friends.getRequests, то отклоняет заявку на добавление в друзья к текущему пользователю. В противном случае удаляет выбранного пользователя из списка друзей текущего пользователя, который может быть получен методом friends.get

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.delete
@return @see info
*/
- (VKRequest *)friendsDeleteWithCustomOptions:(NSDictionary *)options;

/** Возвращает список меток друзей текущего пользователя

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.getLists
@return @see info
*/
- (VKRequest *)friendsGetListsWithCustomOptions:(NSDictionary *)options;

/** Создает новый список друзей у текущего пользователя

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.addList
@return @see info
*/
- (VKRequest *)friendsAddListWithCustomOptions:(NSDictionary *)options;

/** Редактирует существующий список друзей текущего пользователя

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.editList
@return @see info
*/
- (VKRequest *)friendsEditListWithCustomOptions:(NSDictionary *)options;

/** Удаляет существующий список друзей текущего пользователя

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.deleteList
@return @see info
*/
- (VKRequest *)friendsDeleteListWithCustomOptions:(NSDictionary *)options;

/** Возвращает список идентификаторов друзей текущего пользователя, которые установили данное приложение

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.getAppUsers
@return @see info
*/
- (VKRequest *)friendsGetAppUsersWithCustomOptions:(NSDictionary *)options;

/** Возвращает список друзей пользователя, у которых завалидированные или указанные в профиле телефонные номера входят в заданный список.

Использование данного метода возможно только если у текущего пользователя завалидирован номер мобильного телефона. Для проверки этого условия можно использовать метод users.get c параметрами uids=API_USER и fields=has_mobile, где API_USER равен идентификатору текущего пользователя.

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.getByPhones
@return @see info
*/
- (VKRequest *)friendsGetByPhonesWithCustomOptions:(NSDictionary *)options;

/** Отмечает все входящие заявки на добавление в друзья как просмотренные

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.deleteAllRequests
@return @see info
*/
- (VKRequest *)friendsDeleteAllRequestsWithCustomOptions:(NSDictionary *)options;

/** Возвращает список профилей пользователей, которые могут быть друзьями текущего пользователя.

@param options ключи-значения, полный список по этой ссылке: https://vk.com/dev/friends.getSuggestions
@return @see info
*/
- (VKRequest *)friendsGetSuggestionsWithCustomOptions:(NSDictionary *)options;

/** Возвращает информацию о том, добавлен ли текущий пользователь в друзья у указанных пользователей.

@param options ключи-значения, полный список по ссылке: https://vk.com/dev/friends.areFriends
@return @see info
*/
- (VKRequest *)friendsAreFriendsWithCustomOptions:(NSDictionary *)options;

@end

@interface VKUser (Groups)

/**
@name Группы
*/
/** Возвращает информацию о том, является ли пользователь участником сообщества

@param options ключи-значения, полный список в документации: https://vk.com/dev/groups.isMember
@return @see info
*/
- (VKRequest *)groupsIsMemberWithCustomOptions:(NSDictionary *)options;

/** Возвращает информацию о заданном сообществе или о нескольких сообществах

@param options ключи-значения, полный список в документации: https://vk.com/dev/groups.getById
@return @see info
*/
- (VKRequest *)groupsGetByIDWithCustomOptions:(NSDictionary *)options;

/** Возвращает список сообществ указанного пользователя

@param options ключи-значения, полный список в документации: https://vk.com/dev/groups.get
@return @see info
*/
- (VKRequest *)groupsGetWithCustomOptions:(NSDictionary *)options;

/** Возвращает список участников сообщества

@param options ключи-значения, полный список в документации: https://vk.com/dev/groups.getMembers
@return @see info
*/
- (VKRequest *)groupsGetMembersWithCustomOptions:(NSDictionary *)options;

/** Данный метод позволяет вступить в группу, публичную страницу, а также подтвердить участие во встрече.

@param options ключи-значения, полный список в документации: https://vk.com/dev/groups.join
@return @see info
*/
- (VKRequest *)groupsJoinWithCustomOptions:(NSDictionary *)options;

/** Данный метод позволяет выходить из группы, публичной страницы, или встречи

@param options ключи-значения, полный список в документации: https://vk.com/dev/groups.leave
@return @see info
*/
- (VKRequest *)groupsLeaveWithCustomOptions:(NSDictionary *)options;

/** Осуществляет поиск сообществ по заданной подстроке

@param options ключи-значения, полный список в документации: https://vk.com/dev/groups.search
@return @see info
*/
- (VKRequest *)groupsSearchWithCustomOptions:(NSDictionary *)options;

/** Данный метод возвращает список приглашений в сообщества и встречи

@param options ключи-значения, полный список в документации: https://vk.com/dev/groups.getInvites
@return @see info
*/
- (VKRequest *)groupsGetInvitesWithCustomOptions:(NSDictionary *)options;

/** Добавляет пользователя в черный список группы

@param options ключи-значения, полный список в документации: https://vk.com/dev/groups.banUser
@return @see info
*/
- (VKRequest *)groupsBanUserWithCustomOptions:(NSDictionary *)options;

/** Убирает пользователя из черного списка группы

@param options ключи-значения, полный список в документации: https://vk.com/dev/groups.unbanUser
@return @see info
*/
- (VKRequest *)groupsUnbanUserWithCustomOptions:(NSDictionary *)options;

/** Возвращает список забаненных пользователей

@param options ключи-значения, полный список в документации: https://vk.com/dev/groups.getBanned
@return @see info
*/
- (VKRequest *)groupsGetBannedWithCustomOptions:(NSDictionary *)options;

@end