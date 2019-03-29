function handle_sigint {
  echo -e "\nExiting due to SIGINT.\n"
  exit 1
}

function handle_sigterm {
  echo -e "\nExiting due to SIGTERM.\n"
  exit 1
}

trap handle_sigint INT
trap handle_sigterm TERM
