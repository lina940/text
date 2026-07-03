-- =============================================
-- 实验题目：触发器的创建、使用和删除
-- 数据库：学生管理数据库
-- =============================================

USE 学生管理数据库;
GO


-- =============================================
-- 第一阶段：创建表和所有触发器（先不测试）
-- =============================================

-- =============================================
-- 1. 创建表 avgxskc
-- =============================================
PRINT '===== 第1步：创建表 avgxskc =====';
GO

IF OBJECT_ID('avgxskc', 'U') IS NOT NULL
    DROP TABLE avgxskc;
GO

CREATE TABLE avgxskc (
    课程号 VARCHAR(10),
    课程名 NVARCHAR(50) NOT NULL,
    课程成绩平均分 DECIMAL(5,2)
);
GO

-- 插入初始数据
INSERT INTO avgxskc (课程号, 课程名, 课程成绩平均分)
SELECT k.课程号, k.课程名, AVG(CAST(c.成绩 AS DECIMAL(5,2)))
FROM kc k
INNER JOIN cj c ON k.课程号 = c.课程号
GROUP BY k.课程号, k.课程名;
GO

SELECT * FROM avgxskc;
GO


-- =============================================
-- 2. 创建触发器 tri_ins2
-- =============================================
PRINT '===== 创建 tri_ins2 =====';
GO

IF OBJECT_ID('tri_ins2', 'TR') IS NOT NULL
    DROP TRIGGER tri_ins2;
GO

CREATE TRIGGER tri_ins2
ON avgxskc
AFTER INSERT
AS
BEGIN
    PRINT '有新数据插入';
    SELECT * FROM avgxskc;
END;
GO


-- =============================================
-- 3. 创建触发器 tri_ins3
-- =============================================
PRINT '===== 创建 tri_ins3 =====';
GO

IF OBJECT_ID('tri_ins3', 'TR') IS NOT NULL
    DROP TRIGGER tri_ins3;
GO

CREATE TRIGGER tri_ins3
ON avgxskc
AFTER INSERT
AS
BEGIN
    IF EXISTS (
        SELECT 1
        FROM avgxskc a
        INNER JOIN inserted i ON a.课程名 = i.课程名
        WHERE a.课程号 <> i.课程号
    )
    BEGIN
        PRINT '发现课程名重复，请判断是否处理';
    END;
END;
GO


-- =============================================
-- 4. 创建触发器 tri_ins4
-- =============================================
PRINT '===== 创建 tri_ins4 =====';
GO

IF OBJECT_ID('tri_ins4', 'TR') IS NOT NULL
    DROP TRIGGER tri_ins4;
GO

CREATE TRIGGER tri_ins4
ON avgxskc
AFTER INSERT
AS
BEGIN
    DECLARE @cno VARCHAR(10);
    DECLARE @count INT;

    SELECT @cno = 课程号 FROM inserted;

    SELECT @count = COUNT(*) FROM avgxskc WHERE 课程号 = @cno;

    IF @count > 1
    BEGIN
        RAISERROR('课程号%s已经被占用，插入不成功', 16, 1, @cno);
        ROLLBACK TRANSACTION;
        RETURN;
    END
    ELSE
    BEGIN
        PRINT '插入成功';
    END;
END;
GO


-- =============================================
-- 5. 创建替代触发器 tri_ins5
-- =============================================
PRINT '===== 创建 tri_ins5 =====';
GO

IF OBJECT_ID('tri_ins5', 'TR') IS NOT NULL
    DROP TRIGGER tri_ins5;
GO

CREATE TRIGGER tri_ins5
ON avgxskc
INSTEAD OF INSERT
AS
BEGIN
    DECLARE @cno VARCHAR(10);
    DECLARE @cname NVARCHAR(50);
    DECLARE @avg DECIMAL(5,2);

    SELECT @cno = 课程号, @cname = 课程名, @avg = 课程成绩平均分 FROM inserted;

    IF EXISTS (SELECT 1 FROM avgxskc WHERE 课程号 = @cno)
    BEGIN
        PRINT '不可插入';
    END
    ELSE
    BEGIN
        PRINT '插入成功';
        INSERT INTO avgxskc (课程号, 课程名, 课程成绩平均分)
        VALUES (@cno, @cname, @avg);
    END;
END;
GO


-- =============================================
-- 6. 创建触发器 tri_del1
-- =============================================
PRINT '===== 创建 tri_del1 =====';
GO

IF OBJECT_ID('tri_del1', 'TR') IS NOT NULL
    DROP TRIGGER tri_del1;
GO

CREATE TRIGGER tri_del1
ON avgxskc
AFTER DELETE
AS
BEGIN
    DECLARE @cname NVARCHAR(50);
    SELECT @cname = 课程名 FROM deleted;
    PRINT '课程名：' + ISNULL(@cname, '未知') + '的信息已经删除';
END;
GO


-- =============================================
-- 7. 创建触发器 tri_up1
-- =============================================
PRINT '===== 创建 tri_up1 =====';
GO

IF OBJECT_ID('tri_up1', 'TR') IS NOT NULL
    DROP TRIGGER tri_up1;
GO

CREATE TRIGGER tri_up1
ON avgxskc
AFTER UPDATE
AS
BEGIN
    DECLARE @rows INT;
    SET @rows = @@ROWCOUNT;

    IF @rows > 0
        PRINT '更新成功';
    ELSE
        PRINT '更新不成功';
END;
GO

PRINT '===== 所有触发器创建完毕 =====';
GO


-- =============================================
-- 第二阶段：逐一测试触发器
-- 注意：每个测试用 TRY...CATCH 包裹，
--       防止 RAISERROR 中断后续测试
-- =============================================

-- =============================================
-- 测试 tri_ins2：插入一条新课程
-- =============================================
PRINT '===== 测试 tri_ins2 =====';
GO

INSERT INTO avgxskc VALUES('T02', '数据结构', 88.50);
DELETE FROM avgxskc WHERE 课程号 = 'T02';
GO


-- =============================================
-- 测试 tri_ins3：插入重复课程名
-- =============================================
PRINT '===== 测试 tri_ins3 =====';
GO

INSERT INTO avgxskc VALUES('T03', '数据库原理', 75.00);
DELETE FROM avgxskc WHERE 课程号 = 'T03';
GO


-- =============================================
-- 测试 tri_ins4（先禁用 tri_ins5，避免冲突）
-- =============================================
PRINT '===== 测试 tri_ins4 =====';

-- 临时禁用 tri_ins5（INSTEAD OF 会拦截插入，影响 tri_ins4 测试）
DISABLE TRIGGER tri_ins5 ON avgxskc;
GO

PRINT '--- 测试1：插入重复课程号 001 ---';
BEGIN TRY
    INSERT INTO avgxskc VALUES('001', '测试', 75.00);
END TRY
BEGIN CATCH
    PRINT '捕获到错误：' + ERROR_MESSAGE();
END CATCH
GO

PRINT '--- 测试2：插入新课程号 ---';
INSERT INTO avgxskc VALUES('T04', '数据结构', 88.50);
DELETE FROM avgxskc WHERE 课程号 = 'T04';
GO

-- 重新启用 tri_ins5
ENABLE TRIGGER tri_ins5 ON avgxskc;
GO


-- =============================================
-- 测试 tri_ins5
-- =============================================
PRINT '===== 测试 tri_ins5 =====';
GO

PRINT '--- 测试1：插入重复课程号 001 ---';
INSERT INTO avgxskc VALUES('001', '测试', 75.00);
GO

PRINT '--- 测试2：插入新课程号 ---';
INSERT INTO avgxskc VALUES('T05', '计算机网络原理', 82.00);
DELETE FROM avgxskc WHERE 课程号 = 'T05';
GO


-- =============================================
-- 测试 tri_del1
-- =============================================
PRINT '===== 测试 tri_del1 =====';
GO

INSERT INTO avgxskc VALUES('T06', '离散数学', 78.00);
DELETE FROM avgxskc WHERE 课程号 = 'T06';
GO


-- =============================================
-- 测试 tri_up1
-- =============================================
PRINT '===== 测试 tri_up1 =====';
GO

PRINT '--- 测试1：更新存在的记录 ---';
UPDATE avgxskc SET 课程成绩平均分 = 90.00 WHERE 课程号 = '001';
GO

PRINT '--- 测试2：更新不存在的记录 ---';
UPDATE avgxskc SET 课程成绩平均分 = 95.00 WHERE 课程号 = '999';
GO

-- 恢复数据
UPDATE avgxskc SET 课程成绩平均分 = 85.00 WHERE 课程号 = '001';
GO


-- =============================================
-- 第三阶段：查看当前所有触发器
-- =============================================
PRINT '===== 当前所有触发器 =====';
SELECT name AS 触发器名称, type_desc AS 类型, create_date AS 创建日期
FROM sys.objects
WHERE type = 'TR'
ORDER BY name;
GO


-- =============================================
-- 8. 删除今天所有的触发器对象和表 avgxskc
-- =============================================
PRINT '===== 第8步：删除所有触发器和表 avgxskc =====';
GO

-- 删除表（同时删除该表上的所有触发器）
IF OBJECT_ID('avgxskc', 'U') IS NOT NULL
    DROP TABLE avgxskc;
GO

-- 额外清理其他触发器
DECLARE @trigName NVARCHAR(255);
DECLARE @sql NVARCHAR(500);

DECLARE trig_cursor CURSOR FOR
    SELECT name
    FROM sys.objects
    WHERE type = 'TR'
      AND CONVERT(DATE, create_date) = CONVERT(DATE, GETDATE());

OPEN trig_cursor;

FETCH NEXT FROM trig_cursor INTO @trigName;

WHILE @@FETCH_STATUS = 0
BEGIN
    SET @sql = 'DROP TRIGGER ' + @trigName;
    PRINT '删除触发器: ' + @trigName;
    EXEC sp_executesql @sql;
    FETCH NEXT FROM trig_cursor INTO @trigName;
END

CLOSE trig_cursor;
DEALLOCATE trig_cursor;
GO

-- 验证
SELECT name AS 剩余触发器 FROM sys.objects WHERE type = 'TR';
GO

PRINT '===== 全部操作完成 =====';
GO
