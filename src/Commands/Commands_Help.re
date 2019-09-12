open Types;

let message = {
  response_type: "ephemeral",
  text: "How to use /memo",
  attachments:
    Some([|
      {
        fallback: "/memo help",
        title: "/memo help",
        text:
          Some("You've already learned how to get help with `/please help`"),
        color: Some("#ff00ff"),
        pretext: None,
      },
      {
        fallback: "/memo ls",
        title: "/memo ls",
        text:
          Some(":soon:"),
        color: Some("#000000"),
        pretext: None,
      },
    |]),
};

let handle = () => {
  (Help, message);
};
