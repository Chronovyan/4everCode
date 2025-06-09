# Chronovyan Examples

This directory contains practical examples demonstrating how to use Chronovyan in various scenarios. Each example is self-contained and includes detailed comments.

## Table of Contents

1. [Basic Examples](#basic-examples)
2. [Web Development](#web-development)
3. [Data Processing](#data-processing)
4. [Concurrent Programming](#concurrent-programming)
5. [System Programming](#system-programming)
6. [Game Development](#game-development)
7. [Command-Line Tools](#command-line-tools)
8. [Networking](#networking)
9. [Algorithms](#algorithms)
10. [Contributing](#contributing)

## Basic Examples

### Hello World

```chronovyan
// hello_world.chrono
FUNC main() {
    println("Hello, Chronovyan!");
}
```

### Fibonacci Sequence

```chronovyan
// fibonacci.chrono
FUNC fibonacci(n: INT) -> INT {
    MATCH n {
        0 => 0,
        1 => 1,
        _ => fibonacci(n - 1) + fibonacci(n - 2)
    }
}

FUNC main() {
    FOR i IN 0..10 {
        println("fibonacci({i}) = {fibonacci(i)}");
    }
}
```

### File I/O

```chronovyan
// file_io.chrono
IMPORT std.io as io;
IMPORT std.fs as fs;

FUNC main() -> RESULT<(), io::Error> {
    // Write to a file
    LET mut file = TRY(fs::File::create("example.txt"));
    TRY(io::write!(file, "Hello, file!\n"));
    
    // Read from a file
    LET contents = TRY(fs::read_to_string("example.txt"));
    println!("File contents: {contents}");
    
    OK(())
}
```

## Web Development

### HTTP Server

```chronovyan
// web_server.chrono
IMPORT std.net as net;
IMPORT std.io as io;

FUNC handle_connection(mut stream: net::TcpStream) -> io::Result<()> {
    LET response = "HTTP/1.1 200 OK\r\n\
                  <!DOCTYPE html>
                  <html>
                    <head>
                      <title>Chronovyan Server</title>
                    </head>
                    <body>
                      <h1>Hello from Chronovyan!</h1>
                      <p>The current time is: {chrono::Local::now()}</p>
                    </body>
                  </html>";
    
    TRY(io::write!(stream, "{response}"));
    Ok(())
}

FUNC main() -> io::Result<()> {
    LET listener = net::TcpListener::bind("127.0.0.1:8080")?;
    println!("Server running on http://localhost:8080");
    
    FOR stream in listener.incoming() {
        MATCH stream {
            Ok(stream) => {
                std::thread::spawn(|| {
                    LET _ = handle_connection(stream);
                });
            }
            Err(e) => eprintln!("Connection failed: {e}"),
        }
    }
    
    Ok(())
}
```

## Data Processing

### CSV Processing

```chronovyan
// csv_processor.chrono
IMPORT std.io as io;
IMPORT std.fs as fs;
IMPORT std.csv as csv;

STRUCT Person {
    name: String,
    age: u32,
    email: String,
}

FUNC main() -> io::Result<()> {
    LET csv_data = "name,age,email\n\
                   Alice,30,alice@example.com\n\
                   Bob,25,bob@example.com";
    
    LET mut rdr = csv::Reader::from_reader(csv_data.as_bytes());
    
    FOR result in rdr.deserialize() {
        LET person: Person = result?;
        println!("Name: {}, Age: {}, Email: {}", 
                person.name, person.age, person.email);
    }
    
    Ok(())
}
```

## Concurrent Programming

### Parallel Map

```chronovyan
// parallel_map.chrono
IMPORT std.thread as thread;
IMPORT std.sync as sync;

FUNC parallel_map<T, U, F>(input: VEC<T>, f: F) -> VEC<U>
WHERE
    T: Send + 'static,
    U: Send + 'static,
    F: Fn(T) -> U + Send + Copy + 'static,
{
    LET (tx, rx) = sync::mpsc::channel();
    LET mut handles = VEC![];
    
    FOR (i, item) IN input.into_iter().enumerate() {
        LET tx = tx.clone();
        LET handle = thread::spawn(move || {
            LET result = f(item);
            tx.send((i, result)).unwrap();
        });
        handles.push(handle);
    }
    
    // Close the sender
    drop(tx);
    
    // Collect results in the correct order
    LET mut results = VEC![];
    FOR (i, result) IN rx {
        results.push((i, result));
    }
    
    // Sort by original index
    results.sort_by_key(|&(i, _)| i);
    
    // Extract results in order
    results.into_iter().map(|(_, v)| v).collect()
}

FUNC main() {
    LET numbers = VEC![1, 2, 3, 4, 5];
    
    LET squared = parallel_map(numbers, |x| {
        println!("Processing {x} on thread {:?}", thread::current().id());
        x * x
    });
    
    println!("Results: {squared:?}");
}
```

## System Programming

### Process Management

```chronovyan
// process_manager.chrono
IMPORT std.process as process;
IMPORT std.io as io;

FUNC main() -> io::Result<()> {
    // Run a command
    LET output = process::Command::new("ls")
        .arg("-l")
        .arg("-a")
        .output()?;
    
    IF output.status.success() {
        LET stdout = String::from_utf8_lossy(&output.stdout);
        println!("Command output:\n{stdout}");
    } ELSE {
        LET stderr = String::from_utf8_lossy(&output.stderr);
        eprintln!("Command failed: {stderr}");
    }
    
    Ok(())
}
```

## Game Development

### Simple 2D Game

```chronovyan
// game.chrono
IMPORT sdl2;

STRUCT Game {
    canvas: sdl2::render::Canvas<sdl2::video::Window>,
    event_pump: sdl2::EventPump,
    x: i32,
    y: i32,
}

IMPL Game {
    FUNC new() -> Result<Self, String> {
        LET sdl_context = sdl2::init()?;
        LET video_subsystem = sdl_context.video()?;
        
        LET window = video_subsystem.window("Chronovyan Game", 800, 600)
            .position_centered()
            .build()
            .map_err(|e| e.to_string())?;
            
        LET canvas = window.into_canvas().build().map_err(|e| e.to_string())?;
        LET event_pump = sdl_context.event_pump()?;
        
        Ok(Self {
            canvas,
            event_pump,
            x: 400,
            y: 300,
        })
    }
    
    FUNC run(&mut self) -> Result<(), String> {
        LET mut running = true;
        
        WHILE running {
            FOR event IN self.event_pump.poll_iter() {
                MATCH event {
                    sdl2::event::Event::Quit { .. } => {
                        running = false;
                    }
                    sdl2::event::Event::KeyDown { keycode: Some(key), .. } => {
                        MATCH key {
                            sdl2::keyboard::Keycode::Left => self.x -= 10,
                            sdl2::keyboard::Keycode::Right => self.x += 10,
                            sdl2::keyboard::Keycode::Up => self.y -= 10,
                            sdl2::keyboard::Keycode::Down => self.y += 10,
                            _ => {}
                        }
                    }
                    _ => {}
                }
            }
            
            // Clear the screen
            self.canvas.set_draw_color(sdl2::pixels::Color::RGB(0, 0, 0));
            self.canvas.clear();
            
            // Draw a rectangle
            self.canvas.set_draw_color(sdl2::pixels::Color::RGB(255, 0, 0));
            LET rect = sdl2::rect::Rect::new(self.x - 25, self.y - 25, 50, 50);
            self.canvas.fill_rect(rect)?;
            
            // Update the screen
            self.canvas.present();
            
            // Cap the frame rate
            std::thread::sleep(std::time::Duration::new(0, 1_000_000_000u32 / 60));
        }
        
        Ok(())
    }
}

FUNC main() -> Result<(), String> {
    LET mut game = Game::new()?;
    game.run()
}
```

## Command-Line Tools

### CLI Argument Parser

```chronovyan
// cli_app.chrono
IMPORT std.env as env;

STRUCT Config {
    input: String,
    output: Option<String>,
    verbose: bool,
    count: u32,
}

FUNC parse_args() -> Result<Config, String> {
    LET args: VEC<String> = env::args().collect();
    LET mut config = Config {
        input: String::new(),
        output: None,
        verbose: false,
        count: 1,
    };
    
    LET mut i = 1;
    WHILE i < args.len() {
        MATCH args[i].as_str() {
            "-i" | "--input" => {
                i += 1;
                IF i >= args.len() {
                    RETURN Err("Missing argument for --input".to_string());
                }
                config.input = args[i].clone();
            }
            "-o" | "--output" => {
                i += 1;
                IF i >= args.len() {
                    RETURN Err("Missing argument for --output".to_string());
                }
                config.output = Some(args[i].clone());
            }
            "-v" | "--verbose" => {
                config.verbose = true;
            }
            "-c" | "--count" => {
                i += 1;
                IF i >= args.len() {
                    RETURN Err("Missing argument for --count".to_string());
                }
                config.count = args[i].parse().map_err(|_| "Invalid count".to_string())?;
            }
            "-h" | "--help" => {
                println!("Usage: {} [OPTIONS]\n\
                         Options:\n\
                           -i, --input FILE    Input file\n\
                           -o, --output FILE   Output file\n\
                           -v, --verbose       Enable verbose output\n\
                           -c, --count NUM     Number of times to process\n\
                           -h, --help          Print help",
                        args[0]);
                std::process::exit(0);
            }
            arg => {
                RETURN Err(format!("Unknown argument: {arg}"));
            }
        }
        i += 1;
    }
    
    IF config.input.is_empty() {
        RETURN Err("No input file specified".to_string());
    }
    
    Ok(config)
}

FUNC main() {
    MATCH parse_args() {
        Ok(config) => {
            println!("Processing '{}' ({} times)", config.input, config.count);
            IF LET Some(output) = config.output {
                println!("Writing to '{}'", output);
            }
            IF config.verbose {
                println!("Verbose mode enabled");
            }
        }
        Err(e) => {
            eprintln!("Error: {e}");
            std::process::exit(1);
        }
    }
}
```

## Networking

### TCP Chat Server

```chronovyan
// chat_server.chrono
IMPORT std.net as net;
IMPORT std.io as io;
IMPORT std.sync as sync;
USE std::sync::Arc;

TYPE Tx = sync::mpsc::Sender<String>;
TYPE Rx = sync::mpsc::Receiver<String>;

STRUCT Shared {
    clients: VEC<Tx>,
}

IMPL Shared {
    FUNC new() -> Self {
        Shared {
            clients: VEC![],
        }
    }
    
    FUNC register(&mut self, tx: Tx) {
        self.clients.push(tx);
    }
    
    FUNC broadcast(&self, message: &str) {
        FOR client IN &self.clients {
            IF LET Err(e) = client.send(message.to_string()) {
                eprintln!("Failed to send message: {e}");
            }
        }
    }
}

FUNC handle_client(
    mut stream: net::TcpStream,
    shared: Arc<sync::Mutex<Shared>>,
) -> io::Result<()> {
    LET addr = stream.peer_addr()?;
    println!("New connection: {addr}");
    
    LET (tx, rx) = sync::mpsc::channel();
    {
        LET mut shared = shared.lock().unwrap();
        shared.register(tx);
    }
    
    shared.lock().unwrap().broadcast(&format!("* {addr} has joined the chat"));
    
    LET reader = io::BufReader::new(stream.try_clone()?);
    LET mut writer = io::BufWriter::new(stream);
    
    FOR line IN reader.lines() {
        LET line = line?;
        IF line.is_empty() {
            CONTINUE;
        }
        
        LET message = format!("[{addr}] {line}");
        println!("{message}");
        shared.lock().unwrap().broadcast(&message);
    }
    
    println!("Connection closed: {addr}");
    shared.lock().unwrap().broadcast(&format!("* {addr} has left the chat"));
    
    Ok(())
}

FUNC main() -> io::Result<()> {
    LET listener = net::TcpListener::bind("127.0.0.1:8080")?;
    println!("Chat server running on 127.0.0.1:8080");
    
    LET shared = Arc::new(sync::Mutex::new(Shared::new()));
    
    FOR stream IN listener.incoming() {
        LET shared = shared.clone();
        std::thread::spawn(move || {
            IF LET Err(e) = handle_client(stream.unwrap(), shared) {
                eprintln!("Error handling client: {e}");
            }
        });
    }
    
    Ok(())
}
```

## Algorithms

### Binary Search Tree

```chronovyan
// bst.chrono
USE std.cmp::Ordering;

ENUM Tree<T: Ord> {
    Node {
        value: T,
        left: Box<Tree<T>>,
        right: Box<Tree<T>>,
    },
    Empty,
}

IMPL<T: Ord> Tree<T> {
    FUNC new() -> Self {
        Tree::Empty
    }
    
    FUNC insert(&mut self, value: T) {
        MATCH self {
            Tree::Node { ref val, ref mut left, ref mut right } => {
                MATCH value.cmp(val) {
                    Ordering::Less => left.insert(value),
                    Ordering::Greater => right.insert(value),
                    Ordering::Equal => {}
                }
            }
            Tree::Empty => {
                *self = Tree::Node {
                    value,
                    left: Box::new(Tree::Empty),
                    right: Box::new(Tree::Empty),
                };
            }
        }
    }
    
    FUNC contains(&self, value: &T) -> bool {
        MATCH self {
            Tree::Node { val, left, right } => {
                MATCH value.cmp(val) {
                    Ordering::Less => left.contains(value),
                    Ordering::Greater => right.contains(value),
                    Ordering::Equal => true,
                }
            }
            Tree::Empty => false,
        }
    }
}

FUNC main() {
    LET mut tree = Tree::new();
    
    FOR i IN VEC![4, 2, 6, 1, 3, 5, 7] {
        tree.insert(i);
    }
    
    FOR i IN 0..10 {
        println!("Tree contains {i}: {}", tree.contains(&i));
    }
}
```

## Contributing

We welcome contributions to these examples! If you'd like to add a new example or improve an existing one, please follow these guidelines:

1. **Code Style**: Follow the Chronovyan style guide
2. **Documentation**: Include clear comments and a brief description
3. **Testing**: Ensure your example works as expected
4. **Simplicity**: Keep examples focused and easy to understand
5. **Portability**: Ensure examples work across different platforms

To contribute:

1. Fork the repository
2. Create a new branch for your example
3. Add your example in the appropriate directory
4. Update this README if needed
5. Submit a pull request

## License

These examples are licensed under the [MIT License](LICENSE).
