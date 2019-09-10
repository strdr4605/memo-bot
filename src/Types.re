[@decco]
type slack_command_payload = {
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

[@decco]
type attachment = {
  title: string,
  text: string,
  pretext: string,
  color: string,
  fallback: string,
};

[@bs.deriving jsConverter]
[@decco]
type slack_command_response = {
  response_type: string,
  text: string,
  // attachments: option(array(attachment))
};
