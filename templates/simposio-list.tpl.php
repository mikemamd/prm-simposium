<div class="simposiums-records">
  <?php if( isset($content[1]) && !empty($content[1]) ): ?> 
    <h2>Simposium Abiertos</h2>
    <table class="table table-hover simposiums-records">
      <thead>
        <tr class="table-success">
          <td>Simposio</td>
          <td>Fecha</td>
          <td>Lista de Asistencia</td>
        </tr>
      </thead>
      <tbody>
        <?php foreach($content[1] as $simposio): ?> 
          <tr>
            <td><?php print $simposio['title']; ?></td>
            <td><?php print $simposio['date']; ?></td>
            <td><?php print $simposio['view']; ?></td>
          </tr>
        <?php endforeach; ?>
      </tbody>
    </table>
  <?php endif; ?>

  <?php if( isset($content[0]) && !empty($content[0]) ): ?> 
    <h2>Simposium Finalizados</h2>
    <table class="table table-hover simposiums-records">
      <thead>
        <tr class="table-danger">
          <td>Simposio</td>
          <td>Fecha</td>
          <td>Lista de Asistencia</td>
          <td>EXPORTAR</td>
        </tr>
      </thead>
      <tbody>
        <?php foreach($content[0] as $simposio): ?> 
          <tr>
            <td><?php print $simposio['title']; ?></td>
            <td><?php print $simposio['date']; ?></td>
            <td><?php print $simposio['view']; ?></td>
            <td><?php print $simposio['export']; ?></td>
          </tr>
        <?php endforeach; ?>
      </tbody>
    </table>
  <?php endif; ?>

</div>