-module(sync).

-export([createLock/0, lock/1, unlock/1, destroyLock/1]).
-export([createSem/1, semP/1, semV/1, destroySem/1]).
-export([testLock/0, testSem/0]).



createLock() ->
    spawn(fun() -> lock_loop(free, []) end).

lock_loop(free, Queue) ->
    receive
        {lock, From} ->
            From ! ok,
            lock_loop(busy, Queue);

        destroy ->
            ok
    end;

lock_loop(busy, Queue) ->
    receive
        {lock, From} ->
            lock_loop(busy, Queue ++ [From]);

        {unlock, _From} ->
            case Queue of
                [] ->
                    lock_loop(free, []);

                [Next | Rest] ->
                    Next ! ok,
                    lock_loop(busy, Rest)
            end;

        destroy ->
            ok
    end.

lock(L) ->
    L ! {lock, self()},
    receive
        ok ->
            ok
    end.

unlock(L) ->
    L ! {unlock, self()}.

destroyLock(L) ->
    L ! destroy.



createSem(N) when N >= 0 ->
    spawn(fun() -> sem_loop(N, []) end);

createSem(_) ->
    throw(nMustBeNonNegative).

sem_loop(Count, Queue) ->
    receive
        {down, From} ->
            if
                Count > 0 ->
                    From ! ok,
                    sem_loop(Count - 1, Queue);

                true ->
                    sem_loop(0, Queue ++ [From])
            end;

        up ->
            case Queue of
                [] ->
                    sem_loop(Count + 1, []);

                [Next | Rest] ->
                    Next ! ok,
                    sem_loop(Count, Rest)
            end;

        destroy ->
            ok
    end.

semP(S) ->
    S ! {down, self()},
    receive
        ok ->
            ok
    end.

semV(S) ->
    S ! up.

destroySem(S) ->
    S ! destroy.



f(L, W) ->
    lock(L),
    io:format("uno ~p~n", [self()]),
    io:format("dos ~p~n", [self()]),
    io:format("tre ~p~n", [self()]),
    io:format("cua ~p~n", [self()]),
    unlock(L),
    W ! finished.

waiter(L, 0) ->
    destroyLock(L);

waiter(L, N) ->
    receive
        finished ->
            waiter(L, N - 1)
    end.

testLock() ->
    L = createLock(),
    W = spawn(fun() -> waiter(L, 3) end),

    spawn(fun() -> f(L, W) end),
    spawn(fun() -> f(L, W) end),
    spawn(fun() -> f(L, W) end),

    ok.



sem(S, W) ->
    semP(S),

    io:format("uno ~p~n", [self()]),
    io:format("dos ~p~n", [self()]),
    io:format("tre ~p~n", [self()]),
    io:format("cua ~p~n", [self()]),
    io:format("cin ~p~n", [self()]),
    io:format("sei ~p~n", [self()]),

    semV(S),
    W ! finished.

waiter_sem(S, 0) ->
    destroySem(S);

waiter_sem(S, N) ->
    receive
        finished ->
            waiter_sem(S, N - 1)
    end.

testSem() ->
    S = createSem(2),
    W = spawn(fun() -> waiter_sem(S, 5) end),

    spawn(fun() -> sem(S, W) end),
    spawn(fun() -> sem(S, W) end),
    spawn(fun() -> sem(S, W) end),
    spawn(fun() -> sem(S, W) end),
    spawn(fun() -> sem(S, W) end),

    ok.