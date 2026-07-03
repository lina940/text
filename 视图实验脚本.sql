-- =============================================
-- 实验题目：正确运用所学知识，获取数据
-- 实验内容：视图的创建、修改、使用和删除
-- =============================================

-- =============================================
-- 第一步：创建数据库
-- =============================================
CREATE DATABASE 学生管理数据库;
GO

USE 学生管理数据库;
GO

-- =============================================
-- 第二步：创建基础表
-- =============================================

-- 创建学生情况表
CREATE TABLE xsqk (
    学号 VARCHAR(20) PRIMARY KEY,
    姓名 NVARCHAR(20) NOT NULL,
    性别 NVARCHAR(2),
    出生日期 DATE,
    专业 NVARCHAR(50)
);
GO

-- 创建课程表
CREATE TABLE kc (
    课程号 VARCHAR(10) PRIMARY KEY,
    课程名 NVARCHAR(50) NOT NULL
);
GO

-- 创建成绩表
CREATE TABLE cj (
    学号 VARCHAR(20),
    课程号 VARCHAR(10),
    成绩 INT,
    PRIMARY KEY(学号, 课程号),
    FOREIGN KEY(学号) REFERENCES xsqk(学号),
    FOREIGN KEY(课程号) REFERENCES kc(课程号)
);
GO

-- =============================================
-- 第三步：插入测试数据
-- =============================================

-- 插入学生数据
INSERT INTO xsqk VALUES('001', '张明', '男', '2000-03-15', '计算机网络');
INSERT INTO xsqk VALUES('002', '李华', '女', '2001-07-20', '软件工程');
INSERT INTO xsqk VALUES('003', '王芳', '女', '2000-11-08', '计算机网络');
INSERT INTO xsqk VALUES('004', '刘强', '男', '1999-05-25', '信息安全');
INSERT INTO xsqk VALUES('005', '陈伟', '男', '2000-08-10', '计算机网络');
GO

-- 插入课程数据
INSERT INTO kc VALUES('001', '数据库原理');
INSERT INTO kc VALUES('003', '计算机网络');
INSERT INTO kc VALUES('005', '操作系统');
GO

-- 插入成绩数据
INSERT INTO cj VALUES('001', '001', 85);
INSERT INTO cj VALUES('001', '003', 72);
INSERT INTO cj VALUES('001', '005', 68);
INSERT INTO cj VALUES('002', '003', 55);
INSERT INTO cj VALUES('002', '005', 45);
INSERT INTO cj VALUES('003', '003', 78);
INSERT INTO cj VALUES('003', '005', 82);
INSERT INTO cj VALUES('004', '003', 90);
INSERT INTO cj VALUES('004', '005', 88);
INSERT INTO cj VALUES('005', '003', 65);
INSERT INTO cj VALUES('005', '005', 58);
GO


-- =============================================
-- 任务1：利用窗口模式，创建视图v_xscj1，存放及格学生的学号、姓名、课程号、成绩
-- =============================================
-- 窗口模式指使用SQL Server Management Studio的图形界面创建
-- 以下是对应的T-SQL代码：

IF OBJECT_ID('v_xscj1', 'V') IS NOT NULL
    DROP VIEW v_xscj1;
GO

CREATE VIEW v_xscj1
AS
SELECT xsqk.学号, xsqk.姓名, cj.课程号, cj.成绩
FROM xsqk
INNER JOIN cj ON xsqk.学号 = cj.学号
WHERE cj.成绩 >= 60;
GO

-- 验证视图
SELECT * FROM v_xscj1;
GO


-- =============================================
-- 任务2：利用T-SQL，创建视图v_xscj2，存放不及格学生的学号、姓名、课程号、成绩
-- =============================================
IF OBJECT_ID('v_xscj2', 'V') IS NOT NULL
    DROP VIEW v_xscj2;
GO

CREATE VIEW v_xscj2
AS
SELECT xsqk.学号, xsqk.姓名, cj.课程号, cj.成绩
FROM xsqk
INNER JOIN cj ON xsqk.学号 = cj.学号
WHERE cj.成绩 < 60;
GO

-- 验证视图
SELECT * FROM v_xscj2;
GO


-- =============================================
-- 任务3：利用T-SQL，创建视图v_xsqk，存放学生的基本情况，学号、姓名、性别、年龄、专业
-- =============================================
IF OBJECT_ID('v_xsqk', 'V') IS NOT NULL
    DROP VIEW v_xsqk;
GO

CREATE VIEW v_xsqk
AS
SELECT 学号, 姓名, 性别,
       YEAR(GETDATE()) - YEAR(出生日期) AS 年龄,
       专业
FROM xsqk;
GO

-- 验证视图
SELECT * FROM v_xsqk;
GO


-- =============================================
-- 任务4：利用T-SQL，创建视图v_avgcj，存放每个课程的平均分，显示课程号、课程名、平均分
-- =============================================
IF OBJECT_ID('v_avgcj', 'V') IS NOT NULL
    DROP VIEW v_avgcj;
GO

CREATE VIEW v_avgcj
AS
SELECT kc.课程号, kc.课程名, AVG(cj.成绩) AS 平均分
FROM kc
INNER JOIN cj ON kc.课程号 = cj.课程号
GROUP BY kc.课程号, kc.课程名;
GO

-- 验证视图
SELECT * FROM v_avgcj;
GO


-- =============================================
-- 任务5：利用T-SQL，创建视图v_35cj，存放003和005课程都及格的学生的个人信息
-- =============================================
IF OBJECT_ID('v_35cj', 'V') IS NOT NULL
    DROP VIEW v_35cj;
GO

CREATE VIEW v_35cj
AS
SELECT xsqk.学号, xsqk.姓名, xsqk.性别, xsqk.专业
FROM xsqk
WHERE xsqk.学号 IN (
    SELECT a.学号
    FROM cj a
    INNER JOIN cj b ON a.学号 = b.学号
    WHERE a.课程号 = '003' AND a.成绩 >= 60
      AND b.课程号 = '005' AND b.成绩 >= 60
);
GO

-- 验证视图
SELECT * FROM v_35cj;
GO


-- =============================================
-- 任务6：修改视图v_xsqk，只显示计算机网络的学生，并且开启插入数据检查
-- =============================================
ALTER VIEW v_xsqk
WITH CHECK OPTION
AS
SELECT 学号, 姓名, 性别,
       YEAR(GETDATE()) - YEAR(出生日期) AS 年龄,
       专业
FROM xsqk
WHERE 专业 = '计算机网络';
GO

-- 验证修改后的视图
SELECT * FROM v_xsqk;
GO


-- =============================================
-- 任务7：向v_xsqk插入一条合格的数据
-- =============================================
-- 由于视图包含计算列（年龄），需要向基础表插入完整数据
INSERT INTO xsqk(学号, 姓名, 性别, 出生日期, 专业)
VALUES('20240001', '张三', '男', '2000-01-01', '计算机网络');
GO

-- 验证插入
SELECT * FROM v_xsqk WHERE 学号 = '20240001';
GO


-- =============================================
-- 任务8：删除第7题插入的数据
-- =============================================
DELETE FROM xsqk WHERE 学号 = '20240001';
GO

-- 验证删除
SELECT * FROM v_xsqk WHERE 学号 = '20240001';
GO


-- =============================================
-- 任务9：通过视图v_xscj2向xsqk表成功插入一条数据
-- =============================================
-- 先插入学生信息
INSERT INTO xsqk(学号, 姓名, 性别, 出生日期, 专业)
VALUES('20240002', '李四', '男', '2001-05-15', '软件工程');
GO

-- 再插入不及格成绩记录
INSERT INTO cj(学号, 课程号, 成绩)
VALUES('20240002', '001', 55);
GO

-- 通过视图验证
SELECT * FROM v_xscj2 WHERE 学号 = '20240002';
GO


-- =============================================
-- 任务10：加密视图v_35cj
-- =============================================
IF OBJECT_ID('v_35cj', 'V') IS NOT NULL
    DROP VIEW v_35cj;
GO

CREATE VIEW v_35cj
WITH ENCRYPTION
AS
SELECT xsqk.学号, xsqk.姓名, xsqk.性别, xsqk.专业
FROM xsqk
WHERE xsqk.学号 IN (
    SELECT a.学号
    FROM cj a
    INNER JOIN cj b ON a.学号 = b.学号
    WHERE a.课程号 = '003' AND a.成绩 >= 60
      AND b.课程号 = '005' AND b.成绩 >= 60
);
GO

-- 验证视图仍然可用
SELECT * FROM v_35cj;
GO

-- 尝试查看加密视图的定义（会显示加密或无内容）
EXEC sp_helptext 'v_35cj';
GO


-- =============================================
-- 任务11：用命令查看今天建立的所有视图
-- =============================================
SELECT name AS 视图名称, create_date AS 创建日期
FROM sys.objects
WHERE type = 'V'
  AND CONVERT(DATE, create_date) = CONVERT(DATE, GETDATE())
ORDER BY create_date;
GO


-- =============================================
-- 任务12：删除所有今天添加的视图
-- =============================================
IF OBJECT_ID('v_xscj1', 'V') IS NOT NULL
    DROP VIEW v_xscj1;
GO

IF OBJECT_ID('v_xscj2', 'V') IS NOT NULL
    DROP VIEW v_xscj2;
GO

IF OBJECT_ID('v_xsqk', 'V') IS NOT NULL
    DROP VIEW v_xsqk;
GO

IF OBJECT_ID('v_avgcj', 'V') IS NOT NULL
    DROP VIEW v_avgcj;
GO

IF OBJECT_ID('v_35cj', 'V') IS NOT NULL
    DROP VIEW v_35cj;
GO

-- 验证删除结果
SELECT name AS 视图名称, create_date AS 创建日期
FROM sys.objects
WHERE type = 'V'
ORDER BY create_date;
GO
