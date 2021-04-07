<?php
/**
 * @file
 * Generate admin forms to configuration of module
 */

/**
 * Implements hook_form()
 * Form to configurate the hrs diference of simposiums
 */
function prm_simposium_hours_form() {
  $form  = [];
  $hrs   = variable_get('simposiums_hrs', '');
  $close = variable_get('simposiums_close', '');
  
  $form['simposium'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Configuración de diferencia de Hrs'),
    '#weight'      => 1,
    '#collapsible' => FALSE, 
    '#collapsed'   => FALSE
  );
  $form['simposium']['simposiums_hrs'] = array(
    '#type'          => 'textfield',
    '#title'         => t( 'Diferencia de Hrs' ),
    '#description'   => t('Capture el formato de diferencia de Hrs para el despliegue de Simposiums'),
    '#default_value' => !empty($hrs) ? $hrs : '',
    '#rows'          => 1,
    '#format'        => 'textfield',
  );
  $form['simposium']['simposiums_close'] = array(
    '#type'          => 'textfield',
    '#title'         => t( 'Limite de Asistencia' ),
    '#description'   => t('Limite de hrs para cerrar la captura de asistencia en simposiums'),
    '#default_value' => !empty($close) ? $close : '',
    '#rows'          => 1,
    '#format'        => 'textfield',
  );

  return system_settings_form($form);
}

/**
 * Function to validate params on URL.
 * @param ints in url params
 *   - int ($simp) nid of simposium.
 *   - int ($uid) uid of user
 * @return boolean ($validate) 
 * @see prm_simposium_adduser_form().
 */
function prm_simposium_validate_statuser($simp, $uid) {
  $validate['validate'] = 0;

  // Validate the params
  if ( is_numeric($simp) && is_numeric($uid) ) {
    $node = node_load($simp);
    $user = user_load($uid);
    // Consult if user don't exist on linst
    $qry = db_select('list_simposium', 'ls');
    $qry->condition('ls.simposium_uid', $uid);
    $qry->condition('ls.simposium_nid', $simp);
    $qry->fields('ls', ['id', 'simposium_uid']);
    $result = $qry->execute()->fetchAll();

    // Validate node status and user information
    if ( isset($node->status) && $node->status == 1 && !empty($result) && !empty($user) ) {
      foreach ($user->field_user_simposio['und'] as $simposio) {
        if ($simposio['target_id'] == $simp) {
          $name  = $user->field_user_last_name['und'][0]['value'].' ';
          $name .= $user->field_user_mother_last_name['und'][0]['value'].' ';
          $name .= $user->field_user_name['und'][0]['value'];
          
          $validate = [
            'validate' => 1,
            'name'     => $name,
            'mail'     => $user->mail,
          ];
        } // endif
      } // endforeach
    } // endif

  } // endif

  return $validate;
}

/**
 * Implements hook_form()
 * Callback form to confirm the reigster assistance of user
 * @param $simp (int) url param of simposium
 *        $uid  (int) url param of user id
 * @see prm_simpsium_menu()
 */
function prm_simposium_adduser_form($form, &$form_state, $simp = null, $uid = null) {
  $form = [];
  // Validate the correct params and status of simposium and user
  $validate = prm_simposium_validate_statuser($simp, $uid);

  if ( $validate['validate'] == 0 ) {
    drupal_set_message('Error en la solicitud ingresada. Verifique por favor.', 'warning');
    return;
  } // endif

  // Construct the confirm form
  $form['assistance_simp'] = array(
    '#type'  => 'hidden',
    '#value' => $simp,
  );
  $form['assistance_user'] = array(
    '#type'  => 'hidden',
    '#value' => $uid,
  );
  $form['#submit'][] = 'prm_simposium_adduser_form_submit';
  
  return confirm_form(
    $form,
    'Confirmar registro de asistencia',
    'simposiums/asistencia/'.$simp, 
    '¿Confirmar asistecia de <b>'.$validate['name'].' | '.$validate['mail'].'</b>?',
    'Confirmar', 
    'Cancelar'
  );
}

/**
 * Implements hook_form_submit()
 * Callback to form. Add user to list_simposium table.
 * @see prm_simposium_adduser_form().
 */
function prm_simposium_adduser_form_submit($form, &$form_state) {
  // Load the current user to asign the changes
  global $user;
  $mod  = $user->uid;
  // Load the user values  
  $simp = $form_state['values']['assistance_simp'];
  $uid  = $form_state['values']['assistance_user'];

  // Update the user to table registers    
  try {
    $update = db_update('list_simposium')
      ->fields(array(
          'simposium_assistence' => 1,
          'simposium_moderator'  => $mod,
          'date_confirm'         => time(),
        ))
      ->condition('simposium_uid', $uid, '=')
      ->condition('simposium_nid', $simp, '=')
      ->execute();

    drupal_set_message('Asistencia de usuario confirmada con éxito.', 'status');
  }
  catch (Exception $e) {
    watchdog("WATCHDOG_ERROR", t('Error al guardar registro: '.$e));
  } // endexception

  $form_state['redirect'] = 'simposiums/asistencia/'.$simp;
}

/**
 * Implements hook_form()
 * Callback form to confirm delete of user
 * @param $simp (int) url param of simposium
 *        $uid  (int) url param of user id
 * @see prm_simpsium_menu()
 */
function prm_simposium_deleteuser_form($form, &$form_state, $simp = null, $uid = null) {
  $form = [];
  // Validate the correct params and status of simposium and user
  $validate = prm_simposium_validate_statuser($simp, $uid);

  if ( $validate['validate'] == 0 ) {
    drupal_set_message('Error en la solicitud ingresada. Verifique por favor.', 'warning');
    return;
  } // endif

  // Construct the confirm form
  $form['assistance_simp'] = array(
    '#type'  => 'hidden',
    '#value' => $simp,
  );
  $form['assistance_user'] = array(
    '#type'  => 'hidden',
    '#value' => $uid,
  );
  $form['#submit'][] = 'prm_simposium_deleteuser_form_submit';
  
  return confirm_form(
    $form,
    'Retiro de Registro',
    'simposiums/asistencia/'.$simp, 
    '¿Confirmar el retiro de asistecia de <b>'.$validate['name'].' | '.$validate['mail'].'</b>?',
    'Confirmar', 
    'Cancelar'
  );
}

/**
 * Implements hook_form_submit()
 * Callback to form. Delete user to list_simposium table.
 * @see prm_simposium_deleteuser_form().
 */
function prm_simposium_deleteuser_form_submit($form, &$form_state) {
  // Load the current user to asign the changes
  global $user;
  $mod = $user->uid;
  // Load the user values 
  $simp = $form_state['values']['assistance_simp'];
  $uid  = $form_state['values']['assistance_user'];

  // Upptate the status of user (0). Add the user moderator action
  try {
    $update = db_update('list_simposium')
      ->fields(array(
          'simposium_assistence' => 0,
          'simposium_moderator'  => $mod,
          'date_confirm'         => time(),
        ))
      ->condition('simposium_uid', $uid, '=')
      ->condition('simposium_nid', $simp, '=')
      ->execute();

    drupal_set_message('Asistencia de usuario retirada con éxito.', 'status');
  }
  catch (Exception $e) {
    watchdog("WATCHDOG_ERROR", t('Error al actualizar asistencia de usuario: '.$e));
  } // endexception

  $form_state['redirect'] = 'simposiums/asistencia/'.$simp;
}
