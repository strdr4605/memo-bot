open Types;

let postMessage: (string, Js.Json.t) => unit =
  (response_url, response_json) => {
    let _ = {
      Js.Promise.(
        Axios.postData(response_url, response_json |> Obj.magic)
        |> then_(response =>
             resolve(
               Js.log(
                 " POST to " ++ response_url ++ " resolves: " ++ response##data,
               ),
             )
           )
        |> catch(error => resolve(Js.log(error)))
      );
    };
    ();
  };

let mentionUser = payload_record =>
  "<@" ++ payload_record.user_id ++ "|" ++ payload_record.user_name ++ ">";

let generateImmediatResponse = payload_record => {
  let response = {
    response_type: "ephemeral",
    text: mentionUser(payload_record) ++ ", I got it! :blush:",
    attachments: None,
  };
  slack_command_response_encode(response);
};

let sendDelayedResponse = payload_record => {
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

  postMessage(
    payload_record.response_url,
    slack_command_response_encode(response),
  );
};

let handlePayload: slack_command_payload => Js.Json.t =
  payload_record => {
    sendDelayedResponse(payload_record);
    generateImmediatResponse(payload_record);
  };
