![Kodi Logo](resources/banner_slim.png)

# Commit Messages

## Why are commit messages important
The git log is a valuable source for developers to
understand code, spot bugs, generate changelogs, etc.
In order to use it effectively commit messages should be
precise about why this code is needed.

Example:
It's by far more valuable to know why a particular variable
has been assigned to 0x4322 not that it has been assigned this value.

Bad commit message:

```
Assigned value 0x4322 to variable xy
```

Good commit message:
```
Fix failing sync on hw platform xy

According to vendor docs value 0x4322 must be
used to initialize a sync sequence.
See docs: .....
```

## Template

```
[Subsystem:] [Part of Subsystem] Short (50 chars or less) summary of changes

More detailed explanatory text, if necessary.  Wrap it to about 72
characters or so.  In some contexts, the first line is treated as the
subject of an email and the rest of the text as the body.  The blank
line separating the summary from the body is critical (unless you omit
the body entirely); tools like rebase can get confused if you run the
two together.

Further paragraphs come after blank lines.

  - Bullet points are okay, too

  - Typically a hyphen or asterisk is used for the bullet, preceded by a
    single space, with blank lines in between, but conventions vary here

[Ticket: Ticketreference if available]
```

Example:

```
Windowing: GBM - make sure mode exists

In order to prevent blackscreens we need to ensure
the chosen mode actually exists prior to actually
do the atomic commit.
```

Possible Subsystems:
VideoPlayer
AudioEngine
Windowing
TO BE CONTINUED

Possible "Part of Subsystem" for Windowing:
X11
Wayland
GBM
RPi
TO BE CONTINUED

