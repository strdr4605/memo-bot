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

let countRequestsInJavascript: (HttpServer.t, unit) => int = [%bs.raw
  {|
    function setupEnpointWithHttpServer(server) {
      let count = 0;
      server.on('request', (req, res) => ++count)
      return () => {
        const result = count;
        count = -1 // reset the count
        return result
      }
    }
  |}
];

let getRequestsCount = countRequestsInJavascript(server);

App.post(app, ~path="/get-request-count") @@
Middleware.from((_, _) =>
  Response.sendString(
    "The server has been called "
    ++ string_of_int(getRequestsCount())
    ++ " times.",
  )
);
