open Types;

let userRegexp = [%re "/<@[A-Z\d]+?\|?.+>/"];
let eventRegexp = [%re "/\s\"(.+)\"/"]

let isUserMention = target => Js.Re.test_(userRegexp, target);

let eventMatchResult = target => Js.Re.exec_(eventRegexp, target);

let mentionUser = payload =>
  "<@" ++ payload.user_id ++ "|" ++ payload.user_name ++ ">";
