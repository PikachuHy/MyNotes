create table if not exists path
(
    id integer primary key autoincrement ,
    name varchar(200),
    parent_id integer default 0,
    create_time timestamp not null default (datetime('now','localtime')),
    update_time timestamp not null default (datetime('now','localtime'))
);