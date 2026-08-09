-module(difusion).
-export([serv_init/0, serv/1, suscribir/0, env_msg/1, desuscribir/0,client/0, client2/0, loopClient/0, loopClient2/0]).

suscribir() ->
    servidor ! {suscribir, self()}.

env_msg(Msg) ->
    servidor ! {envioMsg, Msg}.

desuscribir() ->
    servidor ! {desuscribir, self()}.

serv(ListPids) ->
    receive 
        {suscribir, PidClnt} ->
            io:fwrite("Cliente: ~p se registro ~n", [PidClnt]),
            case lists:member(PidClnt, ListPids) of
                false ->
                    serv([PidClnt | ListPids]);
                true ->
                    serv(ListPids)
                end;
        {desuscribir, PidClnt} ->
            io:fwrite("Cliente: ~p se desuscribe ~n", [PidClnt]),
            ListNew = lists:delete(PidClnt, ListPids),
            serv(ListNew);
        {envioMsg, Msg} ->
            lists:foreach(fun(Pid) -> Pid ! Msg end, ListPids),
            serv(ListPids)
    end.        

loopClient() -> 
  receive
    Msg -> io:fwrite("Soy el cliente ~p y recibi el msg ~p ~n" ,[self() ,Msg]),
    loopClient()
  end.
loopClient2() -> 
    env_msg('hola subs'),
  receive
    Msg -> io:fwrite("Soy el cliente ~p y recibi el msg ~p ~n" ,[self() ,Msg]),
    loopClient()
  end.
client() ->
    suscribir(),
    loopClient().
client2() ->
    suscribir(),
    suscribir(),
    timer:sleep(500),
    loopClient2().

serv_init() ->
    ServPid = spawn(fun() -> serv([]) end),
    register(servidor, ServPid),
    io:fwrite("Servidor ~p spawneado ~n", [ServPid]),
    spawn(fun() -> client() end),
    spawn(fun() -> client() end),
    spawn(fun() -> client2() end),

    timer:sleep(1000),

    ok.