# WebFlow

A structured, block-based syntax for defining HTML documents.

---

## What Is WebFlow

**WebFlow** is a block-structured language that compiles into HTML.

* `tag:` opens an element
* `;` closes an element
* `{}` defines metadata (attributes, content, styles)
* Indentation defines parent–child relationships

---

## Core Syntax Rules

### 1. Element Declaration

```text
tag:
    ...
;
```

### 2. Blocks

Blocks attach metadata to the **current element**.

```text
blockName{values}
```

### 3. Nesting

Child elements are written between a tag and its closing semicolon.

---

## Block Definitions

| Block       | Purpose                  | Example                    | HTML Equivalent                 |
| ----------- | ------------------------ | -------------------------- | ------------------------------- |
| `content{}` | Inner text       | `p: content{hello};`       | `<p>hello</p>`                  |
| `props{}`   | Standard HTML attributes | `input: props{type:text};` | `<input type="text">`           |
| `classes{}` | CSS class list           | `div: classes{container};` | `<div class="container"></div>` |
| `ids{}`     | Element identifier       | `div: ids{main};`          | `<div id="main"></div>`         |
| `styles{}`  | Inline CSS               | `div: styles{top:0};`      | `<div style="top:0;"></div>`    |
| `dataset{}` | `data-*` attributes      | `div: dataset{id:1};`      | `<div data-id="1"></div>`       |

---

## Example

### WebFlow Input

```text
html:
    head:;
    body:
        div:
            dataset{id:hfsUFjhfuhfuxe2b346}
            classes{test}
            ids{main}
            props{contenteditable:true}
            styles{padding:12px}
            content{hi}
        ;
    ;
;
```

---

## How This Is Parsed

* `html:` → `<html>`
* `head:;` → `<head></head>`
* `body:` → `<body>`
* `div:` → `<div>`

  * `dataset{}` → `data-*` attributes
  * `classes{}` → `class="..."`
  * `ids{}` → `id="..."`
  * `props{}` → standard attributes
  * `styles{}` → inline CSS
  * `content{}` → inner text
* `;` closes each element

---

## Generated HTML

```html
<html>
  <head></head>
  <body>
    <div
      data-id="hfsUFjhfuhfuxe2b346"
      class="test"
      id="main"
      contenteditable="true"
      style="padding:12px;"
    >
      hi
    </div>
  </body>
</html>
```

---

## Recommended Constraints
* Join `classes{}` `ids{}` with spaces
* Auto-append `;` in `styles{}`
* Validate attributes in `props{}` against HTML spec



