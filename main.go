package main

import (
	"context"

	"github.com/spf13/cobra"

	"ipex-llm-ollama/cmd"
)

func main() {
	cobra.CheckErr(cmd.NewCLI().ExecuteContext(context.Background()))
}
