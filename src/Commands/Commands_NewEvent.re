open Types;

let createMessage = text => {
  response_type: "ephemeral",
  text,
  attachments: None,
};

let getEventName = result => {
  let matches = Js.Re.captures(result);
  let firstGroupMatch = matches[1];
  let eventName = Js.toOption(firstGroupMatch);
  switch (eventName) {
  | Some(event) => event
  | None => "No event name"
  };
};

let handle:
  (string, slack_command_payload) => (command, slack_command_response) =
  (userMention, payload) => {
    Js.log(payload.text);
    let matchResult = AppUtils.eventMatchResult(payload.text);

    switch (matchResult) {
    | Some(result) => (
        NewEvent,
        createMessage(
          userMention ++ " did *" ++ getEventName(result) ++ "* | _*1*_ time",
        ),
      )
    | None => (Error, Commands_Error.createErrorMessage("No event name"))
    };
  };
