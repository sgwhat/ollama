package main

import (
	"context"

	"github.com/spf13/cobra"

	"ollama/cmd"
)

func main() {
	cobra.CheckErr(cmd.NewCLI().ExecuteContext(context.Background()))
}
