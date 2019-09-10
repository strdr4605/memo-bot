open Express;

let router = Express.router(~caseSensitive=true, ~strict=true, ());
Router.use(router, Middleware.json(~limit=5.0 |> ByteLimit.mb, ()));
Router.use(router, Middleware.urlencoded(~extended=true, ()));

let (>>) = (f, g, x) => x |> f |> g;

let raiseIfNone =
  fun
  | Some(value) => value
  | None => failwith("Body is none");

[@decco]
type slack_payload = {
  token: string,
  team_id: string,
  team_domain: string,
  channel_id: string,
  channel_name: string,
  user_id: string,
  user_name: string,
  command: string,
  text: string,
  response_url: string,
  trigger_id: string,
};

[@bs.deriving jsConverter]
[@decco]
type slack_command_response = {
  response_type: string,
  text: string,
};

let generateSlackJSONResponse = payload_record => {
  let response = {
    response_type: "ephemeral",
    text: "I process the command: " ++ payload_record.command,
  };
  slack_command_response_encode(response);
};

let postMessage = payload_record => {
  let response = {
    response_type: "ephemeral",
    text:
      "<@"
      ++ payload_record.user_id
      ++ "|"
      ++ payload_record.user_name
      ++ "> send me: "
      ++ payload_record.text,
  };

  let _ = {
    Js.Promise.(
      Axios.postData(
        payload_record.response_url,
        {
          slack_command_responseToJs(response);
        },
      )
      |> then_(response =>
           resolve(
             Js.log(
               payload_record.user_name
               ++ " POST to "
               ++ payload_record.response_url
               ++ " resolves: "
               ++
               response##data,
             ),
           )
         )
      |> catch(error => resolve(Js.log(error)))
    );
  };
  ();
};

Router.post(router, ~path="/") @@
Middleware.from(_next =>
  Request.bodyJSON
  >> raiseIfNone
  >> (
    payload_json => {
      Js.log(Js.Json.stringifyWithSpace(payload_json, 2));
      slack_payload_decode(payload_json) |> Belt.Result.getExn;
    }
  )
  >> (
    payload_record => {
      postMessage(payload_record);
      generateSlackJSONResponse(payload_record) |> Response.sendJson;
    }
  )
);
