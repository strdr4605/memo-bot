open Types;

let message = {
  response_type: "ephemeral",
  text: ":cry: Something went wrong!\nTry `/memo help`",
  attachments: None,
};

let createErrorMessage = error => {
  response_type: "ephemeral",
  text: ":cry: Something went wrong!\nTry `/memo help`",
  attachments: Some([|
  {
    fallback: "Error message",
    title: "Error message",
    text:
      Some(error),
    color: Some("#ff0000"),
    pretext: None,
  },
|]),
};

let handle = () => {
  (Error, message);
};
