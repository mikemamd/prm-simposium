<div class="simposiums-records">
  <?php if( $simposio['class'] == 'table-danger' ): ?>
    <h3>Este Simposio ha finalizado.</h3>
  <?php endif; ?>
  <h2><?php print $simposio['title']; ?></h2>
  <?php if(!empty($content)): ?>
    <table class="table table-hover simposiums-records">
      <thead>
        <tr class="<?php print $simposio['class']; ?>">
          <td>Nombre</td>
          <td>E-Mail</td>
          <td>Status</td>
          <?php if( $simposio['class'] == 'table-success' ): ?>
            <td>Registrar/Eliminar</td>
          <?php endif; ?>
        </tr>
      </thead>
      <tbody>
        <?php foreach($content as $member): ?> 
          <tr class="<?php print $member['class']; ?>">
            <td><?php print $member['name']; ?></td>
            <td><?php print $member['mail']; ?></td>
            <td><?php print $member['assist']; ?></td>
            <?php if( $simposio['class'] == 'table-success' ): ?>
              <td><?php print $member['action']; ?></td>
            <?php endif; ?>
          </tr>
        <?php endforeach; ?>
      </tbody>
    </table>
  <?php else: ?>
    <h3 style="text-align: center;">- Sin usuarios registrados -</h3>
  <?php endif; ?>
</div>