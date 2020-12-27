create table if not exists note
(
    id      integer primary key autoincrement ,
    str_id    varchar(20),
    title    varchar(200),
    path integer default 0,
    tags varchar(200),
    trashed integer default 0,
    create_time timestamp not null default (datetime('now','localtime')),
    update_time timestamp not null default (datetime('now','localtime'))
);