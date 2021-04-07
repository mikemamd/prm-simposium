<?php
/**
 * @Package PRM
 * @Info include methods for implements module prm_simposium
 * @Version dev 1.0.0.0
 */

/**
 * Callback of block_view [simposiums_block]
 * Return array to template with simposiums list.
 * @return array theme ($contet) array to template of block, with hook_theme.
 * @see prm_simposium_theme().
 */
function prm_simposium_getlist(){
  $simposiums = [];
  // Consult the simposiums
  $qry = db_select('node', 'n');
  $qry->innerJoin('field_data_field_date', 'fd', 'fd.entity_id = n.nid');
  $qry->condition('n.type', 'simposiums', '=');
  $qry->condition('fd.entity_type', 'node', '=');
  $qry->fields('n', ['title', 'nid', 'status']);
  $qry->fields('fd', ['field_date_value']);
  $qry->orderBy('fd.field_date_value', 'ASC');
  $result = $qry->execute()->fetchAll();

  if (!empty($result)) {
    foreach ($result as $node) {
      $format = prm_simposium_refresh_date('date', $node->field_date_value);
      $date   = $format['date'];
      $url    = l('VER', 'simposiums/asistencia/'.$node->nid, array('attributes' => array('target'=>'_blank')));

      // Creat array with simposiums
      $simposiums[$node->status][$node->nid] = [
        'title' => $node->title,
        'nid'   => $node->nid,
        'date'  => $date,
        'view'  => $url,
      ];
      if ($node->status == 0) {
        $simposiums[$node->status][$node->nid]['export'] = l('EXPORTAR', 'simposiums/list/export/'.$node->nid );
      }// endif

    }// endforeach
  } // endif

  drupal_add_css( drupal_get_path('module', 'prm_simposium') . '/css/simposiums-register.css' );
  $theme = theme('simposiums_block', array('content' => $simposiums));
  return $theme;
}

/**
 * Callback of block_view [simposiums_assist]
 * Return array to template with assistance list of simposium
 * @return array theme ($contet) array to template of block, with hook_theme.
 *  - @see prm_simposium_theme()
 */
function prm_simposium_registerassist() {
  $members  = [];
  // Validate to display list
  $validate = prm_simposium_register_validate();
  if ($validate['validate'] == 0 ) {
    drupal_set_message('Soliciud de lista erronea. Verifique por favor.', 'warning');
    return $members;
  } // endif

  // Array with info of simposium
  $simposio = [
    'title' => $validate['title'],
    'nid'   => $validate['nid'],
    'class' => ($validate['status'] == 1) ? 'table-success' : 'table-danger',
  ];

  // Consult the users on list simposium
  $qry = db_select('list_simposium', 'ls');
  $qry->innerJoin('users', 'us', 'us.uid = ls.simposium_uid');
  $qry->leftJoin('field_data_field_user_name', 'usn', 'usn.entity_id = ls.simposium_uid');
  $qry->leftJoin('field_data_field_user_last_name', 'usln', 'usln.entity_id = ls.simposium_uid');
  $qry->leftJoin('field_data_field_user_mother_last_name', 'usmln', 'usmln.entity_id = ls.simposium_uid');
  $qry->condition('ls.simposium_nid', $validate['nid'], '=');
  $qry->fields('ls', ['simposium_uid', 'simposium_nid', 'simposium_assistence', 'date', 'date_confirm'])
      ->fields('us', ['uid', 'mail'])
      ->fields('usn', ['field_user_name_value'])
      ->fields('usln', ['field_user_last_name_value'])
      ->fields('usmln', ['field_user_mother_last_name_value']);
  $qry->orderBy('usln.field_user_last_name_value', 'ASC');
  $result = $qry->execute()->fetchAll();

  if (!empty($result)) {
    foreach ($result as $register) {
      $members[$register->uid] = [
        'name'  => $register->field_user_last_name_value.' '.$register->field_user_mother_last_name_value.' '.$register->field_user_name_value,
        'mail'  => $register->mail,
        'class' => '',
      ];
      if ($validate['status'] == 1) {
        // Links to actions
        $add = l('Registrar', 'admin/config/prm-simposium/config/add/'.$simposio['nid'].'/'.$register->uid );
        $del = l('Quitar Registro', 'admin/config/prm-simposium/config/delete/'.$simposio['nid'].'/'.$register->uid );
        // Define assistence, update action user and class of display
        $members[$register->uid]['assist'] = ($register->simposium_assistence == 1) ? 'Asistencia Registrada' : '--';
        $members[$register->uid]['action'] = ($register->simposium_assistence == 1) ? $del : $add;
        $members[$register->uid]['class']  = ($register->simposium_assistence == 1) ? 'tr-confirm' : '';
      }
      else {
        $date = date ('Y-m-d H:i', $register->date_confirm);
        $members[$register->uid]['assist'] = ($register->simposium_assistence == 1) ? 'Asistencia Registrada | '.$date : '--';
        $members[$register->uid]['class']  = ($register->simposium_assistence == 1) ? 'tr-confirm' : '';
      } // endif

    } // endforeach
  } // endif

  drupal_add_css( drupal_get_path('module', 'prm_simposium') . '/css/simposiums-register.css' );
  $theme = theme('simposiums_assist', array('content' => $members, 'simposio' => $simposio));
  return $theme;
}

/**
 * Validate url params, user perms, and simposium status to show list of users register.
 * @return array ($validate). Array with the info of simposium
 *  - [validate] => 1 | 0 (if the param is correct)
 *  - [status]   => 1 | 0 (status of node, publish or unpublish)
 *  - [nid]      => $nid  (nid of simposium)
 *  - [title]    => $title (Name of Node/Simposio)
 * @see prm_simposium_registerassist()
 */
function prm_simposium_register_validate() {
  $validate = [ 'validate' => 0 ];
  // Load the URL params
  $path     = request_path();
  $params   = explode('/', $path);

  // Validate the param
  if ( isset($params[2]) && is_numeric( $params[2] )) {
    $node = node_load($params[2]);
    // Validate node type
    if ( isset($node->type) && $node->type == 'simposiums') {
      $validate = [
        'validate' => 1,
        'status'   => $node->status,
        'nid'      => $node->nid,
        'title'    => $node->title,
      ];
    } // endif
  }// endif

  return $validate;
}

/**
 * Callback to generate a CSV File with information of assitence list of simposium
 * @return file ($csv) File to download
 * @param  INT ($simp) NID of simposium to consult
 * @see prm_simposium_menu()
 */
function prm_simposium_exportlist($simp) {
  $csv = [];
  // Consult the users on list simposium
  $qry = db_select('list_simposium', 'ls');
  $qry->innerJoin('users', 'us', 'us.uid = ls.simposium_uid');
  $qry->innerJoin('node', 'n', 'n.nid = ls.simposium_nid');
  $qry->leftJoin('field_data_field_user_name', 'usn', 'usn.entity_id = ls.simposium_uid');
  $qry->leftJoin('field_data_field_user_last_name', 'usln', 'usln.entity_id = ls.simposium_uid');
  $qry->leftJoin('field_data_field_user_mother_last_name', 'usmln', 'usmln.entity_id = ls.simposium_uid');
  $qry->condition('ls.simposium_nid', $simp, '=');
  $qry->fields('ls', ['simposium_uid', 'simposium_nid', 'simposium_assistence', 'date', 'simposium_moderator', 'date_confirm'])
      ->fields('us', ['uid', 'mail'])
      ->fields('usn', ['field_user_name_value'])
      ->fields('usln', ['field_user_last_name_value'])
      ->fields('usmln', ['field_user_mother_last_name_value'])
      ->fields('n', ['title']);
  $qry->orderBy('usln.field_user_last_name_value', 'ASC');
  $result = $qry->execute()->fetchAll();

  if (!empty($result)) {
    $i    = 1;
    $mods = [];
    // Create Headers to rows
    $csv[0] = [ 'Simposio', 'UID', 'Nombre', 'E-Mail', 'Fecha de registro', 'Asistencia', 'Moderador', 'Confirmación | Modificación' ];

    foreach ($result as $row) {
      $status = '--';
      $name   = $row->field_user_last_name_value.' '.$row->field_user_mother_last_name_value.' '.$row->field_user_name_value;

      // Validate moderator to load or save info
      if ( $row->simposium_moderator != $row->simposium_uid ) {
        if ( isset($mods[$row->simposium_moderator]) === FALSE ) {
          $mods[$row->simposium_moderator] = user_load($row->simposium_moderator)->mail;
        } // endif

        // Save the status of assistence
        $status = ($row->simposium_assistence == 1) ? 'Asistencia Registrada' : 'Asistencia Retirada';
      } // endif

      // Save info on array
      $csv[$i] = [
        $row->title,
        $row->uid,
        $name,
        $row->mail,
        format_date($row->date),
        $status,
        ( isset($mods[$row->simposium_moderator]) ) ? $mods[$row->simposium_moderator] : '--',
        ($row->date != $row->date_confirm) ? format_date($row->date_confirm) : '--',
      ];
      ++$i;
    } // endforeach

    // Headers to download
    header('Content-Type: application/octet-stream');
    header("Content-Transfer-Encoding: Binary");
    header("Content-disposition: attachment; filename=\"reporte_asisstencia.csv\"");

    // Wrapper to out file (open new file)
    $outputBuffer = fopen("php://output", 'w');

    foreach($csv as $val) {
      fputcsv($outputBuffer, $val);
    } // endforeach

    // Close the file
    fclose($outputBuffer);
    exit;
  }
  else {
    drupal_set_message('No se encontraron usuarios registrados en Simposium', 'warning');
    drupal_goto('/simposiums/list');
  } // endif

}

/**
 * Callback of block_view [simposiums_next]
 * Show the next simposiums to user register
 * @return array theme ($contet) array to template of block, with hook_theme.
 * @see prm_simposium_theme().
 */
function prm_simposium_next(){
  $simposiums = $compare = [];
  // Load current user
  global $user;
  if ($user->uid != 0) {
    $user      = user_load($user->uid);
    $user_simp = $user->field_user_simposio;
    // Save the user simposiums to compare after consult
    if (!empty($user_simp['und'])) {
      foreach ($user_simp['und'] as $value) {
        $compare[$value['target_id']] = $value['target_id'];
      } // endforeaxh
    }// endif
  } // endif

  // Consult the simposiums
  $qry = db_select('node', 'n');
  $qry->leftJoin('field_data_field_img_simposium', 'fis', 'fis.entity_id = n.nid');
  $qry->leftJoin('field_data_field_date', 'fd', 'fd.entity_id = n.nid');
  $qry->leftJoin('field_data_field_simposium_limit', 'fsl', 'fsl.entity_id = n.nid');
  $qry->leftJoin('field_data_field_reference', 'fr', 'fr.entity_id = n.nid');
  $qry->leftJoin('field_data_field_content', 'fc', 'fc.entity_id = n.nid');
  $qry->condition('n.status', 1, '=');
  $qry->condition('n.type', 'simposiums', '=');
  $qry->fields('n', ['nid', 'title'])
      ->fields('fis', ['field_img_simposium_fid'])
      ->fields('fd', ['field_date_value'])
      ->fields('fsl', ['field_simposium_limit_value'])
      ->fields('fr', ['field_reference_url'])
      ->fields('fc', ['field_content_value']);
  $qry->orderBy('fd.field_date_value', 'ASC');
  $result = $qry->execute()->fetchAll();

  if ( !empty($result) ) {
    foreach ($result as $simposio) {
      // Limit Date to register on simposium
      $limit = prm_simposium_refresh_date('date', $simposio->field_simposium_limit_value);
      // Load URL of img content
      $file  = file_load($simposio->field_img_simposium_fid);
      $uri   = $file->uri;
      $img   = file_create_url($uri);
      // Construct the linkt to title
      $title = l($simposio->title, $simposio->field_reference_url, array('attributes' => array('target'=>'_blank')));
      // Clean description
      $marca = "<!--corte-->";
      $desc  = $simposio->field_content_value;
      if (strlen($desc) > 180) {
        $desc = wordwrap($desc, 180, $marca);
        $desc = explode($marca, $desc);
        $desc = $desc[0].'...';
      } // endif

      $simposiums[$simposio->nid] = [
        'title'    => $title,
        'desc'     => $desc,
        'img'      => $img,
        'date'     => $limit['date'],
        'register' => ( isset($compare[$simposio->nid]) ) ? 1 : 0,
      ];
      // Action Links
      $add_l = l('Registrarse', 'simposiums/register/add/'.$simposio->nid.'/'.$user->uid, array('attributes' => array('class' => 'btn btn-primary')));
      $add   = ($limit['validate'] == 1) ? $add_l : '<span class="close-register">Fecha limite de registro cerrada</span>';
      $del   = '<span class="register">REGISTRADO</span>';
      $del  .= l('eliminar registro', 'simposiums/register/del/'.$simposio->nid.'/'.$user->uid, array('attributes' => array('class' => 'del-reg')));
      // Send the action
      $simposiums[$simposio->nid]['action'] = ($simposiums[$simposio->nid]['register'] == 0) ? $add : $del;
    } // endofreach
  }// endif

  drupal_add_css( drupal_get_path('module', 'prm_simposium') . '/css/simposiums-register.css' );
  $theme = theme('simposiums_next', array('content' => $simposiums));
  return $theme;
}

/**
 * Callback of menu to add or delete register by user
 * @param
 *  String ($action) add | del => action to update user register
 *  INT ($nid) NID of simposium to register
 *  INT ($uid) User ID to update info
 * @see
 *  prm_simposium_menu().
 *  prm_simposium_next().
 */
function prm_simposium_ownupdate($action, $nid, $uid) {
  $compare  = $limit = [];
  $message  = 'Error en la solicitud ingresada';
  $validate = FALSE;
  // Validate URL params
  if ( $uid == 0 ) {
    $validate = FALSE;
    drupal_goto('/user/register');
  } // endif
  if ($action == 'add' || $action == 'del') {
    $validate = TRUE;
  } // endif
  if ( is_numeric($nid) === FALSE || is_numeric($uid) === FALSE || $validate === FALSE ) {
    $validate = FALSE;
  } // endif

  if ($validate === TRUE ) {
    // Load node and user to validate update
    $simp = node_load($nid);
    $user = user_load($uid);
    if ( isset($simp->type, $simp->status, $user->uid) && $simp->type == 'simposiums' && $simp->status == 1) {
      $limit    = prm_simposium_refresh_date('date', $simp->field_simposium_limit['und'][0]['value']);
      $edit['field_user_simposio']['und'] = $user->field_user_simposio['und'];
      foreach ($edit['field_user_simposio']['und'] as $target) {
        $compare[$target['target_id']] = $target['target_id'];
      }// endoforeach
      $validate = ($limit['validate'] == 1) ? TRUE : FALSE;
    } // endif
    else {
      $validate = FALSE;
    }
  } // endif

  // Add user on list_simposium
  if ( $action == 'add' && $validate === TRUE ) {
    // validate the simposiums user
    if ( isset($compare[$simp->nid]) === FALSE) {
      $edit['field_user_simposio']['und'][] = [ 'target_id' => $simp->nid ];
      try {
        // Save the simposium on user field
        user_save($user, $edit);
        // Insert on list_simposium the user and simposium
        $insert = db_insert('list_simposium')
          ->fields(array(
              'simposium_uid'        => $user->uid,
              'simposium_nid'        => $simp->nid,
              'simposium_assistence' => 0,
              'date'                 => time(),
              'simposium_moderator'  => $user->uid,
              'date_confirm'         => time(),
            ))
          ->execute();

        drupal_set_message('Se registró en Simposio correctamente', 'status');
        drupal_goto('/simposiums/info');
      }
      catch (Exception $e) {
        watchdog("WATCHDOG_ERROR", t('Error al guardar registro: '.$e));
      } // endexception
    }
    else {
      $validate = FALSE;
      $message  = 'Ya se encuentra registrado en Simposio <b>'.$simp->title.'</b>';
    } // endif
  }// endif

  // Delete user on list_simposium
  if ( $action == 'del' && $validate === TRUE && $limit['validate'] == 1 ) {
    if ( isset($compare[$simp->nid]) ) {
      // Eliminate simposium on array to update user field
      unset($compare[$simp->nid]);
      foreach ($compare as $target) {
        $del['field_user_simposio']['und'][] = [ 'target_id' => $target];
      } // endforeach

      try {
        // Save new array to user field
        user_save($user, $del);

        // Delete on DB the records with uid == 0
        $del = db_delete('list_simposium')
          ->condition('simposium_uid', $simp->uid)
          ->condition('simposium_nid', $simp->nid)
          ->execute();

        drupal_set_message('Su registro fue retirado con éxito', 'status');
        drupal_goto('/simposiums/info');
      }
      catch (Exception $e) {
        watchdog("WATCHDOG_ERROR", t('Error al eliminar simposio '.$simp->nid.' de usuario '.$user->uid.' MESSAGE: '.$e));
      } // endexception

    }
    else{
      $validate = FALSE;
      $message  = 'Debe registrarse primero en Simposio <b>'.$simp->title.'</b>';
    } // endif

  } // endif

  if ($validate === FALSE) {
    drupal_set_message($message, 'warning');
    drupal_goto('/simposiums/info');
  }// endif

}

/**
 * Callback of block_view [simposiums_own]
 * Show simposiums to download certified by user
 * @return array theme ($contet) array to template of block, with hook_theme.
 * @see prm_simposium_theme().
 */
function prm_simposium_own(){
  global $user;
  $simposiums = [];

  $qry = db_select('list_simposium', 'ls');
  $qry->innerJoin('node', 'n', 'n.nid = ls.simposium_nid');
  $qry->innerJoin('field_data_field_date', 'fd', 'fd.entity_id = n.nid');
  $qry->leftJoin('field_data_field_simposium_related', 'fsr', 'fsr.field_simposium_related_target_id = n.nid');
  $qry->condition('ls.simposium_uid', $user->uid, '=');
  $qry->condition('ls.simposium_assistence', 1, '=');
  $qry->condition('n.type', 'simposiums', '=');
  $qry->condition('n.status', 0, '=');
  $qry->fields('ls', ['simposium_nid', 'simposium_uid', 'simposium_assistence', 'date_confirm'])
      ->fields('n', ['nid', 'title'])
      ->fields('fd', ['field_date_value'])
      ->fields('fsr', ['entity_id', 'field_simposium_related_target_id']);
  $result = $qry->execute()->fetchAll();

  if ( !empty($result) ) {
    foreach ($result as $simposio) {
      $validate = prm_simposium_refresh_date('close', $simposio->field_date_value);
      if ( $validate['validate'] == 1) {
        $url = l('VER CERTIFICADO', 'print/'.$simposio->entity_id, array('attributes' => array('target'=>'_blank')));
        $simposiums[] = [
          'title'   => $simposio->title,
          'nid'     => $simposio->nid,
          'confirm' => format_date($simposio->date_confirm),
          'action'  => ( !empty($simposio->entity_id) ) ? $url : 'EN PROCESO',
        ];
      } // endif
    } // endforeach
  } // endif

  drupal_add_css( drupal_get_path('module', 'prm_simposium') . '/css/simposiums-register.css' );
  $theme = theme('simposiums_own', array('content' => $simposiums));
  return $theme;
}
