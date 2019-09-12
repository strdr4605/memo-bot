open Types;

let message = {
  response_type: "ephemeral",
  text: ":no_entry_sign: `ls` command is under development :soon:",
  attachments: None
};

let handle = (lsList, _payload) => {
  switch (lsList) {
  | [lsArgument, _] =>
    switch (lsArgument) {
    | _ => (Error, Commands_Error.message)
    }
  | _ => (Ls, message)
  };
};
