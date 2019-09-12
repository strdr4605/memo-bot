open Types;

let message = {
  response_type: "ephemeral",
  text: ":cry: Something went wrong!\nTry `/memo help`",
  attachments: None,
};

let handle = () => {
  (Error, message);
};
