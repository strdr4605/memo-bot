open Express;

/* make a common JSON object representing success */
let makeSuccessJson = () => {
  let json = Js.Dict.empty();
  Js.Dict.set(json, "Memo", Js.Json.string("Bot"));
  Js.Dict.set(json, "success", Js.Json.boolean(true));
  Js.Json.object_(json);
};

let app = express();

App.disable(app, ~name="x-powered-by");

App.use(app, Middleware.from(Cors.middleware));

App.get(app, ~path="/") @@
Middleware.from((_, _) => Response.sendJson(makeSuccessJson()));

App.useRouterOnPath(app, ~path="/v0", Routes.V0.router);

let port =
  "PORT"
  |> Js.Dict.get(Node.Process.process##env)
  |> Js.Option.getWithDefault("3000");

let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    Js.log(e);
    Node.Process.exit(1);
  | _ => Js.log @@ "Listening at PORT:" ++ port
  };

let server = App.listen(app, ~port=int_of_string(port), ~onListen, ());

module MyHttpServer = {
  open HttpServer;

  [@bs.send]
  external on:
    (
      t,
      [@bs.string] [
        | `request((Request.t, Response.t) => unit)
        | `close(unit => unit)
      ]
    ) =>
    unit =
    "on";
};

let countRequests = server => {
  let count = ref(0);
  MyHttpServer.on(server, `request((_, _) => count := count^ + 1));
  () => {
    let result = count^;
    count := (-1);
    result;
  };
};

let getRequestsCount = countRequests(server);

App.post(app, ~path="/get-request-count") @@
Middleware.from((_, _) =>
  Response.sendString(
    "The server has been called "
    ++ string_of_int(getRequestsCount())
    ++ " times.",
  )
);
