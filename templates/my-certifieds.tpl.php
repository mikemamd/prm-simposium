<div class="simposiums-records">
  <?php if( isset($content) && !empty($content) ): ?> 
    <h2>Certificados Disponibles</h2>
    <table class="table table-hover simposiums-records">
      <thead>
        <tr class="table-success">
          <td>Simposio</td>
          <td>Confirmación de Asistencia</td>
          <td>Certificado</td>
        </tr>
      </thead>
      <tbody>
        <?php foreach($content as $simposio): ?> 
          <tr>
            <td><?php print $simposio['title']; ?></td>
            <td><?php print $simposio['confirm']; ?></td>
            <td><?php print $simposio['action']; ?></td>
          </tr>
        <?php endforeach; ?>
      </tbody>
    </table>
  <?php endif; ?>


</div>