create table 'pastes'
  ( 'uuid'    text    primary key
  , 'path '   text    not null
  , 'size'    integer not null
  , 'hash'    text    not null
  , 'created' text    not null
  );

create table 'settings'
  ( 'name'    text    primary key
  , 'value'           not null
  );
