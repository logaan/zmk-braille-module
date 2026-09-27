# TaskDown

1.  This project uses the TaskDown system for managing work.
2.  This system defined tasks in plain text and uses them as the primary means
    of communication between users and agents.

## 1. Tasks folder

1.  A folder containing task folders.

## 2. Task folder

1.  A task has a number and a name. (eg: "01", "keyboard-keys")
2.  This corresponds to a folder. (eg: "tasks/01-keyboard-keys/")
3.  Task folders may contain other task folders.
    1.  These are subtasks that will be completed as part of the parent task.
    2.  Numbers on subtasks should start from `01` and be unique within a task.
    3.  A large project could use subtasks to break work down into phases,
        containing milestones, containing individual steps.
    4.  But often tasks are small and have no need for subtasks.

## 3. Message file

1.  A task folder contains message files.
2.  Each message file
    1.  Has an ID
        1.  This is a ULID.
    2.  Is a markdown file.
        1.  The file name format is "id.md"
        2.  Eg: "01JMQ3ZK7VR8TQ4XNB0W2G9F6E.md"
    3.  Will contain YAML frontmatter
        1.  Described in `3.1. Message root properties`
    4.  May contain a markdown body
        1.  This content is communication between users and agents.
        2.  Messages may communicate instructions, questions, answers, progress
            reports, decisions, results, etc.
        3.  It should be written following `4. Numbered Report Style`

### 3.1. Message root properties

1.  The top level properties in YAML frontmatter are properties of the message
    itself.
2.  Messages may have any properties set.
3.  These are the standard ones:
    1.  `created`
        1.  Required.
        2.  A Datetime in `Sun 27 Sep 13h23` format.
    2.  `author`
        1.  Required.
        2.  A `3.2. Contributor` value.
    3.  `subject`
        1.  Required.
        2.  A string of `64` characters or less.
        3.  Briefly describing the subject of the message.
        4.  This will be shown in listing to help users navigate the
            conversation history.
    4.  `cargo`
        1.  Required.
        2.  Must be one of `3.1.1. Task`, `3.1.2. Question`, or `3.1.3. Answer`

#### 3.1.1. Task

1.  Must have `type: task`
2.  A task may have any properties set.
3.  The standard properties of tasks are as follows:
    1.  `status` is a required `3.1.1.1. Status` value
    2.  `worker` is an optional `3.2. Contributor` value

##### 3.1.1.1. Status

A `Status` value must be one of:

1.  `draft` - Still being written, not ready to begin work.
2.  `ready` - Ready to begin work.
3.  `started` - Work has begun.
4.  `blocked` - Can not be completed by the worker.
5.  `delivered` - Completed, and ready for review.
6.  `done` - Reviewed, no more work required.
7.  `cancelled` - No longer intended to be worked on.

#### 3.1.2. Question

1.  Must have `type: question`
2.  Markdown body
    1.  Must ask just one question.
    2.  If multiple questions need answering they should each be a separate
        message.

#### 3.1.3. Answer

1.  Must have `type: answer`

2.  May have `regarding`

    1.  This is a map of Truncated Task ID -\> Task Subject.

    2.  Task ID should be truncated to 10 characters.

    3.  Task subject should 64 characters or less.

    4.  Eg:

        ``` markdown
        cargo:
          type: answer
          regarding:
            01JMQ3ZK7V: Should we pin the clock in tests?
            01JMQ4B2XD: Which fake clock library should we use?
        ```

3.  Markdown body.

    1.  Should answer to a previous question, or questions.

### 3.2. Contributor

1.  May either be a user or an agent.
2.  A user is represented by the string `user`.
3.  An agent is represented by an object with these properties:
    1.  `type`
        1.  Required.
        2.  Always `agent`.
    2.  `harness`
        1.  Optional.
        2.  Made up of `a-z`, `0-9`, and `-` characters.
        3.  Eg. `pi`
    3.  `model`
        1.  Required.
        2.  Made up of `a-z`, `0-9`, and `-` characters.
        3.  Eg. `claude-opus-5.5`
    4.  `name`
        1.  Optional.
        2.  Used to identify agents and subagents.
        3.  Made up of `a-z`, `0-9`, and `-` characters.
        4.  Eg. `coordinator`, or `researcher-01`

## 4. Numbered Report Style

1.  This file is written in Numbered Report Style.
2.  Each heading, other than the `h1`, is numbered.
3.  Paragraphs are not used.
4.  Content is primarily communicated through nested numbered lists.
5.  Each list item is short and factual.
6.  Short examples are given where appropriate.

## 5. Full example

- `tasks/`
  - `01-keyboard-keys/`
    - `01M3GJ5XDTGG51DB77PR8EJ039.md`

      ``` markdown
      ---
      created: Sun 27 Sep 13h23
      author: user
      subject: Keyboard keys
      cargo:
        type: task
        status: draft
      ---

      # Keyboard keys

      1. Find out what keys are on a macbook.
      ```

    - `01M3GJ5XDTS3PKTY6P4E1J7B1D.md`

      ``` markdown
      ---
      created: Sun 27 Sep 13h24
      author: user
      subject: Keyboard keys
      cargo:
        type: task
        status: ready
      ---
      ```

    - `01M3GJ5XDTH9A69A9H6TZ1MFF6.md`

      ``` markdown
      ---
      created: Sun 27 Sep 13h24
      author:
        type: agent
        harness: pi
        model: kimi-k3
      subject: Keyboard keys
      cargo:
        type: task
        status: started
      ---
      ```

    - `01M3GJ5XDTK7ZP9R2X4V8N6Q3W.md`

      ``` markdown
      ---
      created: Sun 27 Sep 13h34
      author:
        type: agent
        harness: pi
        model: kimi-k3
        name: coordinator
      subject: Results
      cargo:
        type: task
        status: delivered
      ---

      # Results

      1. I have written the Macbook keys to `./output/keys.yaml`
      ```

    - `01M3GJ5XDTM4C8D1E2F3G5H7J9.md`

      ``` markdown
      ---
      created: Mon 28 Sep 09h12
      author: user
      subject: Keyboard keys
      cargo:
        type: task
        status: done
      ---
      ```

## 6. Workflow

1.  Tasks
    1.  Both users and agents can create tasks.
    2.  Users can assign work to themselves or agents.
    3.  Agents can assign work to themselves, users, or other agents.
2.  When agents are assigned to work on tasks
    1.  They should read all messages in the task.
    2.  As they work they may periodically send progress reports.
    3.  They may ask questions, and continue working.
    4.  If they are unable to proceed they are blocked.
        1.  They should check the messages to see if any new messages have
            arrived that unblock them (eg: Answers to questions they've asked).
        2.  If they're still blocked then they should send a message that
            updates the task's status to blocked and explain why.
3.  When an agent completes a task they MUST send a message:
    1.  Updating the status of the task to `delivered`
    2.  Providing the result of task
        1.  OR if the task required making changes elsewhere, then a summary of
            the of the work done.
