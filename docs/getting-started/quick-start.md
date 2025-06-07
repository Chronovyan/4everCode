---
title: Quick-Start
description: Documentation for quick-start
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Quick Start

This guide will help you get up and running with [Chronovyan](https://chronovyan.github.io/h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/) "The) temporal) programming) language) and) runtime") in just a few minutes.

## Basic Usage

###

``cpp
    #

   #include <iostream>

    int main() {
        // Get current time
        auto now = chronovyan::system_clock::now();
        std::cout << "Current time: " << now << std::endl;

        // Create a specific date
        auto date = chronovyan::year_month_day{
            chronovyan::year{2023},
            chronovyan::month{6},
            chronovyan::day{7}
        };
        std::cout << "Specific date: " << date << std::endl;

        // Create a time duration
        auto duration = 2h + 30min;
        std::cout << "Duration: " << duration << std::endl;

        return 0;
    }
```text

###

``cpp
    #include <chronovyan/chronovyan.hpp>

    int main() {
        // Get current time in UTC
        auto utc_time = chronovyan::utc_clock::now();

        // Convert to local time zone
        auto local_zone = chronovyan::current_zone();
        auto local_time = chronovyan::zoned_time{local_zone, utc_time};

        // Format the time
        std::cout << "Local time: " << local_time << std::endl;

        return 0;
    }
```text

## Next Steps

 [API Reference](/).).)/)a)p)i)/)c)h)r)o)n)o).)m)d)
- [Time Zone Guide](/).).)/)g)u)i)d)e)s)/)t)i)m)e)z)o)n)e)s).)m)d)
- [Performance Tips](/).).)/)g)u)i)d)e)s)/)p)e)r)f)o)r)m)a)n)c)e).)m)d)
