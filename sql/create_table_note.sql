create table if not exists note
(
    id      integer primary key autoincrement ,
    title    varchar(200),
    path integer default 0,
    trashed integer default 0,
    create_time timestamp not null default (datetime('now','localtime')),
    update_time timestamp not null default (datetime('now','localtime'))
);