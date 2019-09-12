open Express;
open Types;

let router = Express.router(~caseSensitive=true, ~strict=true, ());
Router.use(router, Middleware.json(~limit=5.0 |> ByteLimit.mb, ()));
Router.use(router, Middleware.urlencoded(~extended=true, ()));

let (>>) = (f, g, x) => x |> f |> g;

let raiseIfNone =
  fun
  | Some(value) => value
  | None => failwith("Body is none");

Router.post(router, ~path="/") @@
Middleware.from(_next =>
  Request.bodyJSON
  >> raiseIfNone
  >> (
    payload_json => {
      Js.log(Js.Json.stringifyWithSpace(payload_json, 2));
      slack_command_payload_decode(payload_json) |> Belt.Result.getExn;
    }
  )
  >> (
    payload_record => {
      Commands.handlePayload(payload_record) |> Response.sendJson;
    }
  )
);
