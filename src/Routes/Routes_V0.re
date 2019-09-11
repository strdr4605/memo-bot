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

let mentionUser = payload_record =>
  "<@" ++ payload_record.user_id ++ "|" ++ payload_record.user_name ++ ">";

let generateSlackJSONResponse = payload_record => {
  let response = {
    response_type: "ephemeral",
    text: mentionUser(payload_record) ++ ", I got it! :blush:",
    attachments: None,
  };
  slack_command_response_encode(response);
};

let postMessage = payload_record => {
  let response = {
    response_type: "ephemeral",
    text: mentionUser(payload_record) ++ " send me: " ++ payload_record.text,
    attachments:
      Some([|
        {
          fallback: "Required plain-text summary of the attachment.",
          title: "MemoBot attachment",
          text: Some("Optional text that appears within the attachment"),
          color: None,
          pretext:
            Some("Optional text that appears above the attachment block"),
        },
      |]),
  };

  let _ = {
    Js.Promise.(
      Axios.postData(
        payload_record.response_url,
        slack_command_response_encode(response) |> Obj.magic,
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
      slack_command_payload_decode(payload_json) |> Belt.Result.getExn;
    }
  )
  >> (
    payload_record => {
      postMessage(payload_record);
      generateSlackJSONResponse(payload_record) |> Response.sendJson;
    }
  )
);
