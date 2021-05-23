create trigger if not exists  [path_auto_update_time]
    after update on path
    for each row
begin
    update path set update_time = (datetime('now','localtime'))
    where id = old.id;
end;