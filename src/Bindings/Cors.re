open Express;

// https://bucklescript.github.io/docs/en/function#solution-guaranteed-uncurrying
[@bs.module]
external corsUncurried: (. unit) => ((. Request.t, Response.t, Next.t) => complete) = "cors";

let middleware = (next, request, response) => {
  corsUncurried(.)(. request, response, next);
};

