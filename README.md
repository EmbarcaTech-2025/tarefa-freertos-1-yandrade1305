
# Tarefa: Roteiro de FreeRTOS #1 - EmbarcaTech 2025

Autor: Yan Andrade de Sena

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Brasília, 12 de junho de 2025

## Reflexão

### O que acontece se todas as tarefas tiverem a mesma prioridade?

O sistema ainda funcionaria, mas com problemas potenciais.  resposta aos botões poderia ficar mais lenta (já que não teria prioridade).Poderiam ocorrer pequenos atrasos nas transições do LED e nos bipes do buzzer.

### Qual tarefa consome mais tempo da CPU?

A tarefa do buzzer, porque fica ativa por 100ms continuamente durante cada bipe. Enquanto as outras tarefas fazem operações muito rápidas (mudança de estado de GPIO).

### Quais seriam os riscos de usar polling sem prioridades?

Usar polling sem prioridades em um sistema RTOS traz riscos como baixa responsividade (podendo perder eventos rápidos), ineficiência no uso da CPU (consumo excessivo verificando estados inalterados), imprevisibilidade nos tempos de resposta (não determinístico), problemas de concorrência (tarefas críticas podem esperar por não-críticas) e debounce ineficiente (leituras instáveis de botões), comprometendo a confiabilidade do sistema.

## 📜 Licença
GNU GPL-3.0.
