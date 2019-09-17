open Types;

module CHelp = Commands_Help;
module CLs = Commands_Ls;
module CError = Commands_Error;
module CNewEvent = Commands_NewEvent;

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

let defaultImmediatResponse: slack_command_payload => slack_command_response =
  payload => {
    response_type: "ephemeral",
    text: AppUtils.mentionUser(payload) ++ ", I got it! :blush:",
    attachments: None,
  };

let sendDelayedResponse = (payload, commandTuple) => {
  switch (commandTuple) {
  | (Ls, message) =>
    postMessage(payload.response_url, slack_command_response_encode(message))
  | (NewEvent, message) =>
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
  ((command, list(string), slack_command_payload)) =>
  (command, slack_command_response) =
  fun
  | (Help, _, _) => CHelp.handle()
  | (Error, _, _) => CError.handle()
  | (Ls, lsList, payload) => CLs.handle(lsList, payload)
  | (NewEvent, [userMention], payload) =>
    CNewEvent.handle(userMention, payload)
  | _ => CHelp.handle();

let parseWithRegexp = (command, payload) =>
  if (AppUtils.isUserMention(command)) {
    (NewEvent, [command], payload);
  } else {
    (Error, [], payload);
  };

let handleTextCommand = (text, payload) => {
  let commandList = text |> Js.String.split(" ") |> Array.to_list;
  Js.log(commandList |> Array.of_list);
  switch (commandList) {
  | [command, ...tail] =>
    switch (command) {
    | "help" => (Help, [], payload)
    | "ls" => (Ls, tail, payload)
    | "" => (Help, [], payload)
    | any => parseWithRegexp(any, payload)
    }
  | _ => (Help, [], payload)
  };
};

let handlePayload: slack_command_payload => Js.Json.t =
  payload => {
    handleTextCommand(payload.text, payload)
    |> handleCommand
    |> generateImmediatResponse(payload);
  };
