create trigger  if not exists [note_auto_update_time]
    after update
    on note
    for each row
begin
    update note
    set update_time = (datetime('now', 'localtime'))
    where id = old.id;
end;