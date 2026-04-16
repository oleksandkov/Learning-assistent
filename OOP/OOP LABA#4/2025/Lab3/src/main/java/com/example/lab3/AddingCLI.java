package com.example.lab3;

import java.util.Scanner;

public class AddingCLI {
    private final World world ;

    public AddingCLI(World world) {
        this.world = world ;
    }

    public void processCommandLine() {
        try (Scanner in = new Scanner(System.in)) {
            while (true) {
                System.out.print("COMMAND>");
                String input = in.nextLine().trim();  //.toLowerCase()
                String[] cmds = input.split(" ");

                if (cmds != null)
                    if (cmds.length == 1) {
                        switch (cmds[0]) {
                            case "escape":
                                world.CLIescape();
                                break;
                            case "exit":
                                world.CLIexit();
                                break;

                        }
                    }
                    else if(cmds.length == 2){

                        if (cmds[0].equals("search")){
                            world.CLIsearch(cmds[1]);
                        }

                    }
                    else if (cmds.length == 3) {
                        if (cmds[0].equals("move")) {

                            world.CLImove(cmds[1], cmds[2]);
                        }
                    } else if (cmds.length == 5) {
                        if (cmds[0].equals("create")) {
                            world.CLIcreate(cmds[1], cmds[2], cmds[3], cmds[4]);
                        }

                    }

            }
        }
    }
}
