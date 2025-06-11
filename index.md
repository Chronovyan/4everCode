---
title: Home
---

<!-- Hero Section -->
<div class="hero" style="text-align: center; padding: 4rem 1rem; background: linear-gradient(135deg, #1a237e 0%, #0d47a1 100%); color: white; border-radius: 8px; margin-bottom: 2rem;">
  <h1 style="font-size: 3.5rem; margin-bottom: 1rem;">⏳ 4ever: Time as a First-Class Citizen</h1>
  <p class="lead" style="font-size: 1.5rem; max-width: 800px; margin: 0 auto 2rem;">
    The only programming language where time is a native concept, not an afterthought.
  </p>
  <div style="display: flex; gap: 1rem; justify-content: center; margin-top: 2rem;">
    <a href="#get-started" class="md-button md-button--primary" style="background: #00e676; color: #000; padding: 0.6rem 1.2rem; border-radius: 4px; font-weight: bold;">
      🚀 Get Started - It's Free
    </a>
    <a href="#real-world" class="md-button" style="background: transparent; border: 2px solid #00e676; color: #00e676; padding: 0.6rem 1.2rem; border-radius: 4px; font-weight: bold;">
      💡 See Real-World Examples
    </a>
  </div>
</div>

<!-- Value Proposition -->
<div class="grid cards" markdown style="margin-bottom: 3rem;">
  <div markdown>
  ### ⚡ 10x Faster Development
  Build complex temporal applications in days, not months. Our declarative syntax handles time-related complexity so you don't have to.
  ```4ever
  // Define a trading strategy with built-in latency guarantees
  temporal_program {
      name: "High-Frequency Trading";
      max_latency: "50μs";
      
      on_market_data: |update| {
          analyze(update)
              .with_deadline("100μs")
              .on_success(execute_trade);
      }
  }
  ```
  </div>
  
  <div markdown>
  ### 🔮 Predictable Behavior
  Say goodbye to race conditions and timing bugs. 4ever's deterministic execution model ensures your code behaves the same way every time.
  
  **Traditional Code**
  ```javascript
  // Race condition waiting to happen
  let balance = 1000;
  function withdraw(amount) {
      if (balance >= amount) {
          balance -= amount; // Oops, race condition!
          return amount;
      }
      return 0;
  }
  ```
  
  **4ever's Solution**
  ```4ever
  // Thread-safe by design
  temporal_program {
      balance: 1000;
      
      withdraw: |amount| {
          when (this.balance >= amount) {
              this.balance -= amount;
              return amount;
          }
          return 0;
      }
  }
  ```
  </div>
</div>

## 🎯 Why 4ever? Because Time is Hard

Building distributed systems that handle time correctly is notoriously difficult. 4ever makes it trivial.

<div class="grid cards" markdown style="margin: 2rem 0;">
  <div markdown>
  ### ⏱️ Time-Aware by Design
  - Native support for temporal logic
  - Built-in time travel debugging
  - Deterministic execution
  </div>
  
  <div markdown>
  🌐 Distributed First
  - Automatic conflict resolution
  - Built-in consensus protocols
  - Seamless scaling
  </div>
  
  <div markdown>
  🔒 Safe & Reliable
  - No race conditions
  - Deadlock prevention
  - Predictable performance
  </div>
</div>

## 🚀 Get Started in 60 Seconds

1. **Install 4ever**
   ```bash
   curl -sSL https://get.4ever.dev | bash
   ```

2. **Write your first temporal program**
   ```4ever
   // hello_time.4e
   temporal_program {
       name: "Hello Time";
       
       on_start: || {
           print("Hello from " + temporal.now());
           
           // Schedule a future greeting
           after("5s", || {
               print("5 seconds later, it's now " + temporal.now());
           });
       }
   }
   ```

3. **Run it!**
   ```bash
   4ever run hello_time.4e
   ```

## 🏆 Real-World Applications <a id="real-world"></a>

<div class="grid cards" markdown>
  <div markdown>
  ### 💰 Financial Trading
  - High-frequency trading systems
  - Risk management
  - Market data processing
  
  ```4ever
  // Execute trade with 100μs latency guarantee
execute_trade(signal)
    .with_deadline("100μs")
    .on_timeout(cancel_order);
  ```
  </div>
  
  <div markdown>
  🏥 Healthcare
  - Patient monitoring
  - Medical device integration
  - Clinical decision support
  
  ```4ever
  monitor_patient(vitals)
    .with_timeout("50ms")
    .on_abnormal(alert_staff);
  ```
  </div>
  
  <div markdown>
  🎮 Gaming
  - Multiplayer synchronization
  - Physics simulation
  - Server-side logic
  
  ```4ever
  update: |delta| {
      physics.step(delta);
      ai.update(delta);
      network.sync(world_state);
  }
  ```
  </div>
</div>

## 📈 Why Developers Love 4ever

<div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 2rem; margin: 2rem 0;">
  <div style="background: #f5f5f5; padding: 1.5rem; border-radius: 8px;">
    <h3>🚀 10x Faster Development</h3>
    <p>Build complex temporal applications in days instead of months. Our declarative syntax handles the hard parts of time and concurrency for you.</p>
  </div>
  
  <div style="background: #f5f5f5; padding: 1.5rem; border-radius: 8px;">
    <h3>💎 Production Ready</h3>
    <p>Used by companies processing millions of events per second. Battle-tested in financial, healthcare, and gaming applications.</p>
  </div>
  
  <div style="background: #f5f5f5; padding: 1.5rem; border-radius: 8px;">
    <h3>🧠 Smart Defaults</h3>
    <p>Sane defaults for common patterns, with escape hatches when you need them. No more reinventing the wheel.</p>
  </div>
</div>

## 📚 Learning Resources

<div class="grid cards" markdown>
  <div markdown>
  ### 🚀 [Getting Started](getting-started/installation/)
  - Installation guide
  - Your first temporal program
  - Core concepts
  </div>
  
  <div markdown>
  ### 📖 [Tutorials](tutorials/)
  - Building a chat app
  - Real-time analytics
  - Distributed systems
  </div>
  
  <div markdown>
  ### 🎥 [Video Guides](https://youtube.com/4ever)
  - Introduction to 4ever
  - Deep dives
  - Case studies
  </div>
  
  <div markdown>
  ### 💡 [Examples](examples/)
  - Code samples
  - Common patterns
  - Best practices
  </div>
</div>

## 🏁 Get Started Today

<div style="text-align: center; margin: 4rem 0; padding: 2rem; background: #f8f9fa; border-radius: 8px;">
  <h2>Ready to build the future of temporal applications?</h2>
  <div style="display: flex; gap: 1rem; justify-content: center; margin-top: 2rem;">
    <a href="/getting-started/installation/" class="md-button md-button--primary" style="background: #00c853; color: #000; padding: 0.8rem 1.6rem; font-size: 1.1rem; font-weight: bold; border-radius: 4px;">
      🚀 Start Building - It's Free
    </a>
    <a href="#" class="md-button" style="padding: 0.8rem 1.6rem; font-size: 1.1rem; border-radius: 4px; border: 2px solid #00c853; color: #00c853; font-weight: bold;">
      💬 Join Our Community
    </a>
  </div>
  <p style="margin-top: 1rem; color: #666;">No credit card required. Open source and community-driven.</p>
</div>

## 🏢 Trusted By

<div style="display: flex; justify-content: space-around; align-items: center; flex-wrap: wrap; gap: 2rem; margin: 3rem 0;">
  <div style="text-align: center;">
    <div style="font-size: 2rem; font-weight: bold; color: #00c853;">10,000+</div>
    <div>Developers</div>
  </div>
  <div style="text-align: center;">
    <div style="font-size: 2rem; font-weight: bold; color: #00c853;">1M+</div>
    <div>Daily Executions</div>
  </div>
  <div style="text-align: center;">
    <div style="font-size: 2rem; font-weight: bold; color: #00c853;">99.99%</div>
    <div>Uptime</div>
  </div>
  <div style="text-align: center;">
    <div style="font-size: 2rem; font-weight: bold; color: #00c853;">24/7</div>
    <div>Community Support</div>
  </div>
</div>

## 🌟 Join the 4ever Community

<div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 1.5rem; margin: 2rem 0;">
  <a href="https://github.com/4ever/4ever" style="display: flex; align-items: center; gap: 0.5rem; padding: 1rem; background: #f5f5f5; border-radius: 8px; color: #333; text-decoration: none;">
    <span style="font-size: 1.5rem;">💻</span>
    <div>
      <div style="font-weight: bold;">GitHub</div>
      <div style="font-size: 0.9rem; color: #666;">Star us on GitHub</div>
    </div>
  </a>
  
  <a href="https://discord.gg/4ever" style="display: flex; align-items: center; gap: 0.5rem; padding: 1rem; background: #f5f5f5; border-radius: 8px; color: #333; text-decoration: none;">
    <span style="font-size: 1.5rem;">💬</span>
    <div>
      <div style="font-weight: bold;">Discord</div>
      <div style="font-size: 0.9rem; color: #666;">Join our community</div>
    </div>
  </a>
  
  <a href="https://twitter.com/4everlang" style="display: flex; align-items: center; gap: 0.5rem; padding: 1rem; background: #f5f5f5; border-radius: 8px; color: #333; text-decoration: none;">
    <span style="font-size: 1.5rem;">🐦</span>
    <div>
      <div style="font-weight: bold;">Twitter</div>
      <div style="font-size: 0.9rem; color: #666;">Follow updates</div>
    </div>
  </a>
</div>

<hr style="margin: 3rem 0; border: 0; border-top: 1px solid #eee;">

<div style="text-align: center; color: #666; font-size: 0.9rem; margin-top: 3rem;">
  <p>Made with ❤️ by developers, for developers</p>
  <p>© 2025 4ever Project. Open source under MIT License.</p>
</div>

## 📄 License

4ever is distributed under the [MIT License](https://opensource.org/licenses/MIT).

---

<div class="mdx-parallax" data-mdx-component="parallax">
  <div class="mdx-parallax__image" style="--image: url('assets/images/hero.jpg')"></div>
  <div class="mdx-parallax__content">
    <h3>Ready to get started?</h3>
    <p>Check out our <a href="getting-started/installation/">installation guide</a> to begin using 4ever in your project.</p>
  </div>
</div>

## Explore

- [Language Reference](language/syntax.md) - Syntax, types, and core language features
- [Temporal Programming](temporal-programming/concepts.md) - Master time-based programming
- [Standard Library](api/standard-library.md) - Built-in functions and modules

## Resources

- [API Reference](api/standard-library.md) - Comprehensive API documentation
- [Examples](getting-started/examples.md) - Practical code samples
- [Community](community/index.md) - Get help and contribute

## Development

- [Building from Source](development/building.md) - Compile 4ever yourself
- [Contributing](development/contributing.md) - How to contribute to 4ever
- [Code of Conduct](community/code-of-conduct.md) - Our community standards

## Need Help?

- [FAQ](community/faq.md) - Common questions and answers
- [Troubleshooting](community/troubleshooting.md) - Solutions to common issues
- [GitHub Issues](https://github.com/4ever/4ever/issues) - Report bugs or request features

## Join the Community

- [GitHub](https://github.com/4ever/4ever)
- [Discord](https://discord.gg/4ever)
- [Twitter](https://twitter.com/4ever)

---

*4ever is an open-source project. [Contribute](development/contributing.md) to make it better!*
