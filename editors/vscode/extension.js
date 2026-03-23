'use strict';

const vscode = require('vscode');
const cp     = require('child_process');
const path   = require('path');
const fs     = require('fs');

// ── Получить путь к компилятору из настроек ───────────────────────────────────

function fluxcPath() {
    const configured = vscode.workspace.getConfiguration('flux').get('compilerPath');
    // Если явно задано в настройках — используем
    if (configured && configured !== 'fluxc') return configured;
    // Ищем fluxc.exe рядом с расширением (bundled)
    const bundled = path.join(__dirname, 'fluxc.exe');
    if (fs.existsSync(bundled)) return bundled;
    // Fallback: ищем в PATH
    return 'fluxc';
}

// ── Провайдер автодополнения ──────────────────────────────────────────────────

const completionProvider = {
    provideCompletionItems(document, position) {
        const fluxc  = fluxcPath();
        const file   = document.fileName;
        const line   = position.line + 1;          // VS Code 0-based → 1-based
        const col    = position.character + 1;

        let output;
        try {
            // Передаём текущее содержимое через временный буфер не нужен:
            // fluxc читает файл с диска. VS Code автосохранение должно быть
            // включено, или используем document.getText() через stdin.
            // Проще всего — сохранить документ перед запросом, но это
            // слишком навязчиво. Вместо этого передаём текст через stdin.
            const text = document.getText();
            output = cp.execSync(
                `"${fluxc}" --complete ${line} ${col} "${file}"`,
                {
                    input:   text,
                    encoding: 'utf8',
                    timeout:  5000,
                    // stderr не прокидываем, чтобы не засорять Output
                    stdio: ['pipe', 'pipe', 'ignore'],
                }
            );
        } catch (_) {
            return [];
        }

        let parsed;
        try   { parsed = JSON.parse(output); }
        catch { return []; }

        if (!Array.isArray(parsed)) return [];

        return parsed.map(item => {
            const ci      = new vscode.CompletionItem(item.label, item.kind);
            ci.detail     = item.detail || '';
            ci.sortText   = item.label; // алфавитная сортировка
            return ci;
        });
    }
};

// ── Активация расширения ──────────────────────────────────────────────────────

function activate(context) {
    // Регистрируем провайдер автодополнения для языка flux
    // Символы-триггеры: '.' — для дот-нотации
    const reg = vscode.languages.registerCompletionItemProvider(
        { language: 'flux', scheme: 'file' },
        completionProvider,
        '.'    // trigger character
    );
    context.subscriptions.push(reg);
}

function deactivate() {}

module.exports = { activate, deactivate };
