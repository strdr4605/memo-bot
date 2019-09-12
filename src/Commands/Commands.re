open Types;

module MHelp = Commands_Help;
module MLs = Commands_Ls;
module MError = Commands_Error;

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

let mentionUser = payload =>
  "<@" ++ payload.user_id ++ "|" ++ payload.user_name ++ ">";

let defaultImmediatResponse: slack_command_payload => slack_command_response =
  payload => {
    response_type: "ephemeral",
    text: mentionUser(payload) ++ ", I got it! :blush:",
    attachments: None,
  };

let sendDelayedResponse = (payload, commandTuple) => {
  switch (commandTuple) {
  | (Ls, message) =>
    postMessage(payload.response_url, slack_command_response_encode(message))
  | _ => ()
  };
};

let generateImmediatResponse = (payload, commandTuple) => {
  sendDelayedResponse(payload, commandTuple);
  let response =
    switch (commandTuple) {
    | (Help, message) => message
    | (Error, message) => message
    | (_, _) => defaultImmediatResponse(payload)
    };
  slack_command_response_encode(response);
};

let handleCommand:
  ((command, list(Js.String.t), slack_command_payload)) =>
  (command, slack_command_response) =
  fun
  | (Help, _, _) => MHelp.handle()
  | (Error, _, _) => MError.handle()
  | (Ls, lsList, payload) => MLs.handle(lsList, payload)
  | _ => MHelp.handle();

let handleTextCommand = (text, payload) => {
  let commandList = text |> Js.String.split(" ") |> Array.to_list;
  Js.log(commandList |> Array.of_list);
  switch (commandList) {
  | [command, ...tail] =>
    switch (command) {
    | "help" => (Help, [], payload)
    | "" => (Help, [], payload)
    | "ls" => (Ls, tail, payload)
    | _ => (Error, [], payload)
    }
  | _ => (Help, [], payload)
  };
};

let handlePayload: slack_command_payload => Js.Json.t =
  payload => {
    // sendDelayedResponse(payload);
    handleTextCommand(payload.text, payload)
    |> handleCommand
    |> generateImmediatResponse(payload);
  };
